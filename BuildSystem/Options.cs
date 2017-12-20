namespace BuildSystem
{
    public class Options
    {
        public string GccOpt { get; set; } = "-masm=intel -m32 -std=gnu99 -fno-builtin -ffreestanding  -w -O2 -Wall -Wextra -nostartfiles -nostdlib -fno-stack-protector";
        public string IncludesDirectory { get; set; } = "headers";
        public string BinDirectory { get; set; } = "bin";
        public string LinkerFile { get; set; } = "linker.ld";
    }
}