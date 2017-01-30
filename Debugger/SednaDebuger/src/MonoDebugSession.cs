/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Linq;
using System.Net;
using Mono.Debugger.Client;
using Microsoft.CSharp.RuntimeBinder;


namespace VSCodeDebug
{
    public class MonoDebugSession : DebugSession
    {
        private const string MONO = "mono";
        private readonly string[] MONO_EXTENSIONS = new String[] {
            ".cs",
            ".fs", ".fsi", ".ml", ".mli", ".fsx", ".fsscript"
        };
        private const int MAX_CHILDREN = 100;

        private System.Diagnostics.Process _process;
      /*  private Handles<object[]> _variableHandles;
        private Handles<object> _frameHandles;
        private object _exception;*/
        private Dictionary<int, Thread> _seenThreads = new Dictionary<int, Thread>();
        private bool _attachMode = false;
        private bool _terminated = false;
        private bool _stderrEOF = true;
        private bool _stdoutEOF = true;
		public static Action<string, object, string> Callback { get; set; }

        public MonoDebugSession() : base(true)
        {

            MonoDebugSession.Callback = (type, threadinfo, text) =>
            {
                int tid;
                switch (type)
                {
                    case "TargetStopped":
                        Stopped();
                        SendEvent(CreateStoppedEvent("step", threadinfo));
                        break;

                    case "TargetHitBreakpoint":
                        Stopped();
                        SendEvent(CreateStoppedEvent("breakpoint", threadinfo));
                        break;

                    case "TargetExited":
                        Terminate("target exited");
                        break;

                    case "Output":
                        SendOutput("stdout", text);
                        break;

                    case "ErrorOutput":
                        SendOutput("stderr", text);
                        break;

                    default:
                        SendEvent(new Event(type));
                        break;
                }
            };

        }

        public override void Initialize(Response response, dynamic args)
        {
            SendResponse(response, new Capabilities()
            {
                // This debug adapter does not need the configurationDoneRequest.
                supportsConfigurationDoneRequest = false,

                // This debug adapter does not support function breakpoints.
                supportsFunctionBreakpoints = false,

                // This debug adapter doesn't support conditional breakpoints.
                supportsConditionalBreakpoints = false,

                // This debug adapter does not support a side effect free evaluate request for data hovers.
                supportsEvaluateForHovers = false,

                // This debug adapter does not support exception breakpoint filters
                exceptionBreakpointFilters = new dynamic[0]
            });

            // Mono Debug is ready to accept breakpoints immediately
            SendEvent(new InitializedEvent());
        }

        public override void Launch(Response response, dynamic args)
        {
            
        }

        public override void Attach(Response response, dynamic args)
        {


            SendResponse(response);
        }

        public override void Disconnect(Response response, dynamic args)
        {

            SendResponse(response);
        }

        public override void Continue(Response response, dynamic args)
        {
            
            SendResponse(response);
        }

        public override void Next(Response response, dynamic args)
        {
            
            SendResponse(response);
        }

        public override void StepIn(Response response, dynamic args)
        {
           
            SendResponse(response);
        }

        public override void StepOut(Response response, dynamic args)
        {
            
            SendResponse(response);
        }

        public override void Pause(Response response, dynamic args)
        {
            
            SendResponse(response);
        }

        public override void SetBreakpoints(Response response, dynamic args)
        {
            string path = null;
            if (args.source != null)
            {
                string p = (string)args.source.path;
                if (p != null && p.Trim().Length > 0)
                {
                    path = p;
                }
            }
            if (path == null)
            {
                SendErrorResponse(response, 3010, "setBreakpoints: property 'source' is empty or misformed", null, false, true);
                return;
            }
            path = ConvertClientPathToDebugger(path);

           /* if (!HasMonoExtension(path))
            {
                // we only support breakpoints in files mono can handle
                SendResponse(response, new SetBreakpointsResponseBody());
                return;
            }*/

            var clientLines = args.lines.ToObject<int[]>();
            HashSet<int> lin = new HashSet<int>();
            for (int i = 0; i < clientLines.Length; i++)
            {
                lin.Add(ConvertClientLineToDebugger(clientLines[i]));
            }

            // find all breakpoints for the given path and remember their id and line number
           /* var bpts = new List<Tuple<int, int>>();
            foreach (var be in Debugger.Breakpoints)
            {
                var bp = be.Value as Mono.Debugging.Client.Breakpoint;
                if (bp != null && bp.FileName == path)
                {
                    bpts.Add(new Tuple<int, int>((int)be.Key, (int)bp.Line));
                }
            }*/



            for (int i = 0; i < clientLines.Length; i++)
            {
                var l = ConvertClientLineToDebugger(clientLines[i]);


                //TODO: Debugger.Breakpoints.Add(id, Debugger.BreakEvents.Add(path, l));
                // Console.WriteLine("added bpt #{0} for line {1}", id, l);
            }


            var breakpoints = new List<Breakpoint>();
            foreach (var l in clientLines)
            {
                breakpoints.Add(new Breakpoint(true, l));
            }

            response.SetBody(new SetBreakpointsResponseBody(breakpoints));
        }

        public override void StackTrace(Response response, dynamic args)
        {
            var stackFrames = new List<StackFrame>();

			
            SendResponse(response, new StackTraceResponseBody(stackFrames));
        }

        public override void Scopes(Response response, dynamic args)
        {

            var scopes = new List<Scope>();


            SendResponse(response, new ScopesResponseBody(scopes));
        }

        public override void Variables(Response response, dynamic args)
        {

            var variables = new List<Variable>();

           	variables.Add(new Variable("some_var", "lol"));

            SendResponse(response, new VariablesResponseBody(variables));
        }

        public override void Threads(Response response, dynamic args)
        {
            var threads = new List<Thread>();
            
            SendResponse(response, new ThreadsResponseBody(threads));
        }

        public override void Evaluate(Response response, dynamic args)
        {
            string error = null;

            SendResponse(response, new EvaluateResponseBody("lol", 0));
        }

        //---- private ------------------------------------------

        private void SendOutput(string category, string data)
        {
            if (!String.IsNullOrEmpty(data))
            {
                if (data[data.Length - 1] != '\n')
                {
                    data += '\n';
                }
                SendEvent(new OutputEvent(category, data));
            }
        }

        private void Terminate(string reason)
        {
            if (!_terminated)
            {

                // wait until we've seen the end of stdout and stderr
                for (int i = 0; i < 100 && (_stdoutEOF == false || _stderrEOF == false); i++)
                {
                    System.Threading.Thread.Sleep(100);
                }

                SendEvent(new TerminatedEvent());

                _terminated = true;
                _process = null;
            }
        }

        private StoppedEvent CreateStoppedEvent(string reason, object ti, string text = null)
        {
            return new StoppedEvent(1, reason, text);
        }

      

        private void Stopped()
        {
            
        }
       
    }
}
