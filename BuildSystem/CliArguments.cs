using EntryPoint;

namespace BuildSystem
{
    public class CliArguments : BaseCliArguments
    {
        public CliArguments() : base("MyvarOS Build System") { }

        [OptionParameter(ShortName: 's', LongName: "src")]
        [Help("The Directory that comains The MyvarOS source code")]
        public string SourceCodeDirectory { get; set; }

        [OptionParameter(ShortName: 'o', LongName: "options")]
        [Help("The json file with the json config.")]
        public string OptionsFile { get; set; }
    }
}