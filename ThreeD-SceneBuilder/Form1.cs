using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Security;
using System.Diagnostics;

namespace ThreeD_SceneBuilder
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void loadSceneButton_Click(object sender, EventArgs e)
        {
            loadSceneDialog.ShowDialog();
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            try
            {
                var filePath = loadSceneDialog.FileName;
                using (Stream str = loadSceneDialog.OpenFile())
                {
                    loadScene(new Scanner(str));
                }
            }
            catch (SecurityException ex)
            {
                MessageBox.Show($"Security error.\n\nError message: {ex.Message}\n\n" +
                $"Details:\n\n{ex.StackTrace}");
            }
        }

        public SceneObject root;
        private int main_cam;

        private void loadScene(Scanner s)
        {
            root = new SceneObject("root", 0);

            Dictionary<int, SceneObject> components = new Dictionary<int, SceneObject>();
            components.Add(0, root);

            int component_count = s.nextInt();

            for (int i = 0; i < component_count; i++)
            {
                int id = s.nextInt();
                int parent = s.nextInt();
                string name = readString(s);
                string type = readString(s);
                SceneObject obj = null;

                Vec3 position, scale, color;
                string path;

                switch (type)
                {
                    case "OBJECT":
                        obj = new SceneObject(name, id);
                        break;
                    case "MESH":
                        path = readString(s);
                        position = readVec(s);
                        scale = readVec(s);

                        obj = new MeshObject(name, id, path, position, scale);
                        break;
                    case "PHYSICAL":
                        obj = new PhysicalObject(name, id);
                        break;
                    case "TERRAIN":
                        int chunk_size = s.nextInt();
                        int chunk_height = s.nextInt();

                        obj = new TerrainObject(name, id, chunk_size, chunk_height);
                        break;
                    case "WATER":
                        position = readVec(s);
                        scale = readVec(s);

                        obj = new WaterObject(name, id, position, scale);
                        break;
                    case "CAMERA":
                        double clip_dist = s.nextDouble();
                        double speed = s.nextDouble();
                        obj = new CameraObject(name, id, clip_dist, speed);
                        break;
                    case "DIRECTIONAL_LIGHT":
                        color = readVec(s);
                        Vec3 direction = readVec(s);

                        obj = new DirectionalLightObject(name, id, color, direction);
                        break;
                    case "POINT_LIGHT":
                        color = readVec(s);
                        position = readVec(s);
                        double radius = s.nextDouble();
                        Vec3 attenuation = readVec(s);

                        obj = new PointLightObject(name, id, color, position, radius, attenuation);
                        break;
                    case "AUDIO_SOURCE":
                        position = readVec(s);

                        obj = new AudioSourceObject(name, id, position);
                        break;
                    case "NUMBER_VALUE":
                        double val_num = s.nextDouble();

                        obj = new NumberValueObject(name, id, val_num);
                        break;
                    case "VECTOR_VALUE":
                        Vec3 val_vec = readVec(s);

                        obj = new Vec3ValueObject(name, id, val_vec);
                        break;
                    case "SCRIPT":
                        path = readString(s);

                        obj = new ScriptObject(name, id, path);
                        break;
                }

                components[parent].addChild(obj);
                components.Add(id, obj);
            }
            main_cam = s.nextInt();

            sceneTree.Nodes.Clear();
            sceneTree.Nodes.Add(root.MakeNode());
            propertyGrid1.SelectedObject = root;
        }

        private string readString(Scanner s)
        {
            int len = s.nextInt();
            return s.readChars(len);
        }

        private Vec3 readVec(Scanner s)
        {
            return new Vec3(s.nextDouble(), s.nextDouble(), s.nextDouble());
        }

        private void saveSceneButton_Click(object sender, EventArgs e)
        {
            saveSceneDialog.ShowDialog();
        }

        private void saveSceneDialog_FileOk(object sender, CancelEventArgs e)
        {
            try
            {
                var filePath = saveSceneDialog.FileName;
                using (Stream str = saveSceneDialog.OpenFile())
                {
                    saveScene(str);
                }
            }
            catch (SecurityException ex)
            {
                MessageBox.Show($"Security error.\n\nError message: {ex.Message}\n\n" +
                $"Details:\n\n{ex.StackTrace}");
            }
        }

        private void write(Stream s, string str)
        {
            char[] c = str.ToCharArray();
            byte[] arr = System.Text.Encoding.UTF8.GetBytes(c);
            s.Write(arr, 0, arr.Length);
        }

        private void saveScene(Stream s)
        {
            List<SceneObject> objects = new List<SceneObject>();
            root.getChildrenRecursive(objects);
            objects.Remove(root);

            write(s, objects.Count.ToString());
            s.WriteByte((byte)'\n');


            foreach (SceneObject obj in objects)
            {
                write(s, obj.ToSaveString());
                s.WriteByte((byte)'\n');
            }

            write(s, main_cam.ToString());

            s.Close();
        }

        private void sceneTree_AfterSelect(object sender, TreeViewEventArgs e)
        {
            propertyGrid1.SelectedObject = e.Node.Tag;
        }
    }

    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class Vec3
    {
        public double x { get; set; }
        public double y { get; set; }
        public double z { get; set; }

        public Vec3(double x, double y, double z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public override string ToString()
        {
            return x + " " + y + " " + z;
        }
    }
}
