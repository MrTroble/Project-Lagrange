﻿using ShaderTool.Command;
using System;
using System.IO;
using static ShaderTool.Error;
using static ShaderTool.Util.Util;

namespace ShaderTool {
    class Program {

        public static string CWD = Environment.CurrentDirectory;
        public static string ResourcesFolder = Path.Combine(CWD, "Resources");
        public static bool console = false;

        public static int Execute(string id, string[] arg) {
            switch (id) {
                case "setcwd":
                    CWD = arg[0];
                    return 0;
                case "settool":
                    CWD = Environment.CurrentDirectory + "../../../../../TGEngine/resources";
                    return 0;
                case "pipe":
                    return Pipe.PipeCommand(arg);
                case "shader":
                    return Shader.ShaderCommand(arg);
                case "texture":
                    return Texture.TextureCommand(arg);
                case "actor":
                    return Actor.ActorCommand(arg);
                case "map":
                    return Map.MapCommand(arg);
                case "material":
                    return Material.MaterialCommand(arg);
                case "exit":
                    Environment.Exit(0);
                    return 0;
            }

            Console.WriteLine("Wrong parameters! Possible: pipe/shader/texture/setcwd/settool/actor/material/exit!");
            return WRONG_PARAMS;
        }

        public static int Main(string[] args) {

            args = GetParamas(args);
            if (args.Length < 1) {
                console = true;
                while (true) {
                    Console.Write(">>> ");
                    args = Console.ReadLine().Split(" ");
                    args = GetParamas(args);
                    AsssertValues(args, 1);
                    int rcode = Execute(args[0], GetParams(args));
                    Console.WriteLine("Exite code " + rcode);
                }
            }
            AsssertValues(args, 1);
            return Execute(args[0], GetParams(args));
        }

        public static string[] GetParamas(string[] args) {
            for (int i = 0; i < args.Length - 1; i++) {
                if (args[i] == "--dir") {
                    string path = args[i + 1];
                    if (path == "tool") {
                        CWD = Environment.CurrentDirectory + "../../../../../TGEngine/resources";
                    } else {
                        CWD = path;
                    }
                    string[] cpy = (string[])args.Clone();
                    args = Array.FindAll(cpy, str => str != "--dir" && str != path);
                    break;
                }
            }
            return args;
        }

    }
}
