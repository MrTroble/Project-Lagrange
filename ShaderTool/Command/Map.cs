﻿using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using ShaderTool.Util;
using static ShaderTool.Error;
using static ShaderTool.Util.Util;

namespace ShaderTool.Command {

    class MapData {
        public string[] actorNames;
        public string[] materialNames;
        public string[] textureNames;
        public Dictionary<string, float> fontNames;
    }

    class Map {

        public const uint TGR_VERSION = 2;
        public const string MAP_FILE_EXTENSION = ".json";
        public const string RESOURCE_FILE_EXTENSION = ".tgr";

        public static int MapCommand(string[] args) {

            if (!AssertValues(args))
                return NOT_ENOUGH_PARAMS;

            switch (args[0]) {
                case "add":
                    return MapAdd(GetParams(args));
                case "rm":
                case "remove":
                    return MapRm(GetParams(args));
                case "make":
                    return MapMake(GetParams(args));
                case "list":
                    return MapList();
                case "addactor":
                    return MapAddActor(GetParams(args));
                case "rmactor":
                case "removeactor":
                    return MapRmActor(GetParams(args));
                case "addfont":
                    return MapAddFont(GetParams(args));
                case "rmfont":
                case "removefont":
                    return MapRmFont(GetParams(args));
            }

            Console.WriteLine("Wrong parameters! Must be add/rm/make/list!");
            return WRONG_PARAMS;

        }

        public static string GetMapFilePath(string mapName) => Path.Combine(Program.ResourcesFolder, mapName + MAP_FILE_EXTENSION);
        public static string GetResourceFilePath(string mapName) => Path.Combine(Program.ResourcesFolder, mapName + RESOURCE_FILE_EXTENSION);

        private static int MapRmFont(string[] args) {
            if (!AssertValues(args, 2))
                return NOT_ENOUGH_PARAMS;

            string mapName = args[0];

            MapData map = Load(mapName);

            if (map == null) {
                Console.WriteLine("Map '{0}' not found", mapName);
                return WRONG_PARAMS;
            }

            for (int i = 1; i < args.Length; i++) {
                if (!map.fontNames.ContainsKey(args[i])) {
                    Console.WriteLine("Font {0} is not part of the map {1}, skipping!", args[i], mapName);
                    continue;
                }
                map.fontNames.Remove(args[i]);
            }

            Save(mapName, map);

            return SUCCESS;
        }

        private static int MapAddFont(string[] args) {
            if (!AssertValues(args, 3))
                return NOT_ENOUGH_PARAMS;

            string mapName = args[0];

            MapData map = Load(mapName);

            if (map == null) {
                Console.WriteLine("Map '{0}' not found", mapName);
                return WRONG_PARAMS;
            }

            if (map.fontNames == null)
                map.fontNames = new Dictionary<string, float>();

            for (int i = 1; i < (args.Length - 1) / 2 + 1; i++) {
                string path = Path.Combine(Program.ResourcesFolder, args[i * 2 - 1]);
                if (!File.Exists(path)) {
                    Console.WriteLine("Font '{0}' not found, skipping", path);
                    continue;
                }

                float fontSize = 0;
                if (!float.TryParse(args[i * 2], out fontSize)) {
                    Console.WriteLine("'{0}' is not a valid float, skipping!", args[i * 2]);
                    continue;
                }

                map.fontNames.Add(args[i * 2 - 1], fontSize);
            }

            Save(mapName, map);

            return SUCCESS;

        }

        public static MapData Load(string mapName) {
            string resourceFilePath = GetMapFilePath(mapName);

            if (!File.Exists(resourceFilePath))
                return null;

            string fileContent = File.ReadAllText(resourceFilePath);
            MapData map = JsonConvert.DeserializeObject<MapData>(fileContent);
            UpdateMaterials(map);

            return map;
        }

        public static void Save(string mapName, MapData map) => File.WriteAllText(GetMapFilePath(mapName), JsonConvert.SerializeObject(map, Program.FORMATTING_MODE));

        public static void UpdateMaterials(MapData map) {

            List<string> newMaterialNames = new List<string>();
            List<string> newTextureNames = new List<string>();

            foreach (string actorName in map.actorNames) {

                string actorPath = Actor.GetFilePath(actorName);
                string actorFileContent = File.ReadAllText(actorPath);
                ActorData actor = JsonConvert.DeserializeObject<ActorData>(actorFileContent);
                string materialName = actor.materialName;

                if (!newMaterialNames.Contains(materialName) && materialName != null)
                    newMaterialNames.Add(materialName);

                Material.Load();

                string diffuseTexture = Cache.MATERIALS[materialName].diffuseTexture;
                if (!newTextureNames.Contains(diffuseTexture) && diffuseTexture != null)
                    newTextureNames.Add(diffuseTexture);

            }

            map.materialNames = newMaterialNames.ToArray();
            map.textureNames = newTextureNames.ToArray();
        }

        public static void AddActors(MapData map, string[] actorsToAdd) {

            if (map.actorNames == null)
                map.actorNames = new string[0];
            if (map.materialNames == null)
                map.materialNames = new string[0];

            // We don't know how many actors are valid so I'm using a list to compensate for that
            List<string> actorNames = map.actorNames.ToList();

            foreach (string actorName in actorsToAdd) {
                string actorPath = Actor.GetFilePath(actorName);

                if (!File.Exists(actorPath)) {
                    Console.WriteLine("Actor '{0}' not found, skipping", actorName);
                    continue;
                }

                if (actorNames.Contains(actorName)) {
                    Console.WriteLine("Actor '{0}' already added, skipping", actorName);
                    continue;
                }

                actorNames.Add(actorName);
                Console.WriteLine("Added Actor '{0}'", actorName);

            }

            map.actorNames = actorNames.ToArray();
            UpdateMaterials(map);
        }

        public static int MapAdd(string[] args) {

            if (!AssertValues(args))
                return NOT_ENOUGH_PARAMS;

            if (!Directory.Exists(Program.ResourcesFolder))
                Directory.CreateDirectory(Program.ResourcesFolder);

            string mapName = args[0];
            string[] actors = GetParams(args);
            string mapFilePath = GetMapFilePath(mapName);

            if (!AssertName(mapName))
                return WRONG_PARAMS;

            if (!File.Exists(mapFilePath))
                File.Create(mapFilePath).Close();

            MapData newMap = new MapData();

            // GetParams() sets the value to null instead of leaving an empty array if the length is 0
            if (actors != null)
                AddActors(newMap, actors);

            Save(mapName, newMap);
            Console.WriteLine("Added new map '{0}'", mapName);
            return SUCCESS;
        }

        public static int MapRm(string[] args) {
            if (!AssertValues(args))
                return NOT_ENOUGH_PARAMS;

            string mapName = args[0];
            string mapFilePath = GetMapFilePath(mapName);

            if (!File.Exists(mapFilePath)) {
                Console.WriteLine("Map '{0}' was not found", mapName);
                return WRONG_PARAMS;
            } else {
                File.Delete(mapFilePath);
                Console.WriteLine("Map '{0}' was deleted", mapName);
                return SUCCESS;
            }
        }

        public static int MapMake(string[] args) {

            if (!AssertValues(args))
                return NOT_ENOUGH_PARAMS;

            string mapName = args[0];
            string mapFilePath = GetMapFilePath(mapName);
            string resourceFilePath = GetResourceFilePath(mapName);

            // .tgr file format documentation:
            // https://troblecodings.com/fileformat.html

            if (!File.Exists(mapFilePath)) {
                Console.WriteLine("'{0}' is not a map.", mapName);
                return WRONG_PARAMS;
            }

            File.Delete(resourceFilePath);

            Stream resourceStream = File.OpenWrite(resourceFilePath);
            resourceStream.Write(BitConverter.GetBytes(TGR_VERSION));

            MapData mapData = Load(mapName);

            Material.Load();

            int status = SUCCESS;
            status = AddTexturesToResource(resourceStream, mapData);
            if (status != SUCCESS)
                return status;

            status = AddMaterialsToResource(resourceStream, mapData);
            if (status != SUCCESS)
                return status;

            status = AddActorsToResource(resourceStream, mapData);
            if (status != SUCCESS)
                return status;

            status = AddFontToResource(resourceStream, mapData);
            if (status != SUCCESS)
                return status;

            status = AddAnimationToResource(resourceStream, mapData);
            if (status != SUCCESS)
                return status;

            resourceStream.Close();

            return SUCCESS;
        }

        private static int AddFontToResource(Stream resourceStream, MapData mapData) {
            if (mapData.fontNames == null)
                mapData.fontNames = new Dictionary<string, float>();
            resourceStream.Write(BitConverter.GetBytes(mapData.fontNames.Count));

            foreach ((string font, float fontSize) in mapData.fontNames) {
                string path = Path.Combine(Program.ResourcesFolder, font);

                if (!File.Exists(path)) {
                    Console.WriteLine("Fontfile {0} not found, skipping!");
                    continue;
                }

                byte[] data = File.ReadAllBytes(path);
                resourceStream.Write(BitConverter.GetBytes(data.Length));
                resourceStream.Write(BitConverter.GetBytes(fontSize));
                resourceStream.Write(data);
            }

            resourceStream.Write(BitConverter.GetBytes(0xFFFFFFFF));
            return SUCCESS;
        }

        private static int AddAnimationToResource(Stream resourceStream, MapData mapData) {
            // Default skip
            resourceStream.Write(BitConverter.GetBytes(0));
            resourceStream.Write(BitConverter.GetBytes(0xFFFFFFFF));
            return SUCCESS;
        }

        public static int MapList() {
            string[] fileList = Directory.GetFiles(Program.ResourcesFolder);
            List<string> filteredList = new List<string>();

            if (fileList.Length == 0) {
                Console.WriteLine("No content added yet.");
            } else {
                foreach (string filePath in fileList) {
                    if (Path.GetFileName(filePath).EndsWith(".json")
                        && !Path.GetFileName(filePath).EndsWith("_Actor.json")
                        && Path.GetFileName(filePath) != "Materials.json") {
                        filteredList.Add(Path.GetFileNameWithoutExtension(filePath));
                    }
                }

                if (filteredList.Count == 0) {
                    Console.WriteLine("No maps added yet.");
                } else {
                    Console.WriteLine("Count: {0}", filteredList.Count);
                    filteredList.ForEach(mapName => Console.WriteLine(" - " + mapName));
                }

            }

            return SUCCESS;
        }

        public static int MapAddActor(string[] args) {
            if (!AssertValues(args, 2))
                return NOT_ENOUGH_PARAMS;

            string mapName = args[0];
            string[] actorsToAdd = GetParams(args);

            MapData map = Load(mapName);

            if (map == null) {
                Console.WriteLine("Map '{0}' not found", mapName);
                return WRONG_PARAMS;
            }

            AddActors(map, actorsToAdd);
            Save(mapName, map);

            return SUCCESS;
        }

        public static int MapRmActor(string[] args) {

            if (!AssertValues(args, 2))
                return NOT_ENOUGH_PARAMS;

            string mapName = args[0];
            string[] actorsToRemove = GetParams(args);

            MapData map = Load(mapName);

            List<string> actorNames = map.actorNames.ToList();

            if (map == null) {
                Console.WriteLine("Map '{0}' not found", mapName);
                return WRONG_PARAMS;
            }

            bool changed = false;

            foreach (string actorName in actorsToRemove) {
                if (actorNames.Contains(actorName)) {
                    actorNames.Remove(actorName);
                    changed = true;
                    Console.WriteLine("Removed actor '{0}' from map '{1}'", actorName, mapName);
                } else {
                    Console.WriteLine("Map '{0}' does not contain actor '{1}', skipping", mapName, actorName);
                }
            }

            if (changed) {
                map.actorNames = actorNames.ToArray();
                UpdateMaterials(map);
                Save(mapName, map);
            }

            return SUCCESS;
        }

        private static int AddTexturesToResource(Stream resourceStream, MapData mapData) {

            string[] mapTextureNames = mapData.textureNames;

            if (mapTextureNames == null)
                return SUCCESS;

            resourceStream.Write(BitConverter.GetBytes(mapData.textureNames.Length));

            foreach (string mapTextureName in mapTextureNames) {

                string textureFilePath = Texture.GetFilePath(mapTextureName);

                if (!File.Exists(textureFilePath)) {
                    Console.WriteLine("'{0}' is not a texture.", mapTextureName);
                    return WRONG_PARAMS;
                }

                byte[] textureData = File.ReadAllBytes(textureFilePath);
                resourceStream.Write(BitConverter.GetBytes(textureData.Length));
                resourceStream.Write(textureData);

            }

            resourceStream.Write(BitConverter.GetBytes(0xFFFFFFFF));

            return SUCCESS;
        }

        // AddTransformsToResource

        private static int AddMaterialsToResource(Stream resourceStream, MapData mapData) {

            resourceStream.Write(BitConverter.GetBytes(mapData.materialNames.Length));

            foreach (string materialName in mapData.materialNames) {

                // Material not found
                if (!Cache.MATERIALS.ContainsKey(materialName)
                  || Cache.MATERIALS[materialName] == null) { // Not sure if this null check is even necessary, but better safe than sorry

                    Console.WriteLine("Material '{0}' was not found!", materialName);
                    continue;

                }

                // Write size of this block in bytes to file
                resourceStream.Write(BitConverter.GetBytes(Material.MATERIAL_SIZE));

                MaterialData materialData = Cache.MATERIALS[materialName];

                // Write color to file, provided as an R,G,B,A float array
                if (materialData.color == null)
                    for (int i = 0; i < 4; i++)
                        resourceStream.Write(BitConverter.GetBytes(1f));
                else
                    for (int i = 0; i < 4; i++)
                        resourceStream.Write(BitConverter.GetBytes(materialData.color[i]));

                // Refresh texture cache
                Cache.TEXTURES = Texture.GetExistingTextureNames();

                // Find index from texture name
                string texture = materialData.diffuseTexture;

                uint textureIndex = 0;

                if (texture == null)
                    Console.WriteLine("No texture provided, using default");
                else
                    textureIndex = (uint)Array.IndexOf(mapData.textureNames, texture);

                // Write diffuse texture to file
                resourceStream.Write(BitConverter.GetBytes(textureIndex));
                resourceStream.Write(BitConverter.GetBytes(0u)); // Animation support?
            }

            resourceStream.Write(BitConverter.GetBytes(0xFFFFFFFF));
            return SUCCESS;

        }

        private static int AddActorsToResource(Stream resourceStream, MapData mapData) {

            resourceStream.Write(BitConverter.GetBytes(mapData.actorNames.Length));

            // Write the data names into the resource file
            foreach (string actorName in mapData.actorNames) {

                ActorData actorData = Actor.Load(actorName);

                uint actorDataSize = (uint)actorData.vertices.Length * 4;
                resourceStream.Write(BitConverter.GetBytes(actorDataSize));

                if ((actorData.localTransform.Length - 16) % 4 != 0) {
                    Console.WriteLine("Wrong transform size, needs to be a multiple of 4");
                }

                uint matrixCount = (uint)Math.Truncate((actorData.localTransform.Length - 16) / 4.0f);
                if (matrixCount == 1) {
                    Console.WriteLine("Possibly wrong instance count, lower than 2. Make sure to include your basic matrix for all objects and the others after the basic matrix.");
                }
                resourceStream.Write(BitConverter.GetBytes(matrixCount));

                // Write the local transform as a 4x4 matrix into the file
                for (int y = 0; y < (16 + matrixCount * 4); y++)
                    resourceStream.Write(BitConverter.GetBytes(actorData.localTransform[y]));

                // Find the material ID from the material name
                byte id = (byte)Array.IndexOf(mapData.materialNames, actorData.materialName);

                // Just take the last material in case none was supplied
                // We can avoid excessive error handling that way
                resourceStream.WriteByte(id);

                // Write the layer id to the file (e.g. 0 for game actors and 1 for UI actors)
                resourceStream.WriteByte(actorData.layerId);

                // Write the index count so that we know how many indices are in the actor
                resourceStream.Write(BitConverter.GetBytes(actorData.indexCount));

                // Write the amount of vertex points into the file
                resourceStream.Write(BitConverter.GetBytes(actorData.vertexCount));

                // Write all indices to file
                if (actorData.indexCount != 0) // Not sure how to handle this?
                    foreach (int index in actorData.indices)
                        resourceStream.Write(BitConverter.GetBytes(index));

                // Write all vertices to the file
                if (actorData.vertexCount != 0) // Not sure how to handle this?
                    foreach (float vertex in actorData.vertices)
                        resourceStream.Write(BitConverter.GetBytes(vertex));

                foreach (Instance instance in actorData.instances) {
                    float[] matrix = CalculateInstanceMatrix(actorData, instance);
                    // Instance map writing
                }

            }

            resourceStream.Write(BitConverter.GetBytes(0xFFFFFFFF));

            return SUCCESS;

        }

        private static float[] CalculateInstanceMatrix(ActorData actorData, Instance instance) {
            float[] matrix = instance.matrix;
            ActorData baseActor = actorData;

            // If using relation actor, use that, otherwise use base actor as relation
            if (instance.relation != "" || instance.relation != null) {
                ActorData relActor = Actor.Load(instance.relation);
                if (relActor != null)
                    baseActor = relActor;
            }

            matrix[0] += baseActor.localTransform[3]; // x
            matrix[1] += baseActor.localTransform[7]; // y

            float relActorWidth = baseActor.localTransform[0];
            float relActorHeight = baseActor.localTransform[5];

            matrix = ShiftMatrixCoordinates(matrix, relActorWidth / 2, relActorHeight / 2, instance.relationAnchor);

            float width = matrix[2];
            float height = matrix[3];
            matrix = ShiftMatrixCoordinates(matrix, width / 2, height / 2, instance.anchor);

            return matrix;
        }

        private static float[] ShiftMatrixCoordinates(float[] matrix, float shiftWidth, float shiftHeight, Anchor anchor) {
            int[] scaleMult = { 0, 0 }; // 0 = don't move, 1 = add, -1 = subtract

            // Read: If anchor is a left/right/top/bottom anchor, then ...
            if (AnchorGroup.LEFT_ANCHORS.Contains(anchor))
                scaleMult[0] = 1;
            else if (AnchorGroup.RIGHT_ANCHORS.Contains(anchor))
                scaleMult[0] = -1;

            if (AnchorGroup.TOP_ANCHORS.Contains(anchor))
                scaleMult[1] = 1;
            else if (AnchorGroup.BOTTOM_ANCHORS.Contains(anchor))
                scaleMult[1] = -1;

            // Anchors with center elements will not move the matrix in their respective direction,
            // as the scale remains 0 because it doesn't pass through any if.

            float[] newMatrix = {
                matrix[0] + shiftWidth * scaleMult[0],
                matrix[1] + shiftHeight * scaleMult[1],
                matrix[2],
                matrix[3]
            };

            return newMatrix;
        }
    }
}
