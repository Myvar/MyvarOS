/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
using System;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace VSCodeDebug
{
    internal class Program
    {
        const int DEFAULT_PORT = 4711;

        private static bool trace_requests;
        private static bool trace_responses;

        private static void Main(string[] argv)
        {
            int port = -1;

            // parse command line arguments
            foreach (var a in argv)
            {
                switch (a)
                {
                    case "--trace":
                        trace_requests = true;
                        break;
                    case "--trace=response":
                        trace_requests = true;
                        trace_responses = true;
                        break;
                }
            }
            RunSession(Console.OpenStandardInput(), Console.OpenStandardOutput());
        }


        private static void RunSession(Stream inputStream, Stream outputStream)
        {
            DebugSession debugSession = new MonoDebugSession();
            debugSession.TRACE = trace_requests;
            debugSession.TRACE_RESPONSE = trace_responses;
            debugSession.Start(inputStream, outputStream).Wait();
        }

    }
}