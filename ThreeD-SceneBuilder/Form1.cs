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

        private SceneObject root;

        private void loadScene(Scanner s)
        {
            root = new SceneObject("root", 0);

            Dictionary<int, SceneObject> components = new Dictionary<int, SceneObject>();
            components.Add(0, root);

            int component_count = s.nextInt();

            for(int i=0;i<component_count; i++)
            {
                int id = s.nextInt();
                int parent = s.nextInt();
                string name = readString(s);
                string type = readString(s);
                SceneObject obj = null;

                Vec3 position, scale, color;
                string path;

                switch(type)
                {
                    case "OBJECT":
                        obj = new SceneObject(name, id);
                        break;
                    case "MESH":
                        path = readString(s);
                        position = readVec(s);
                        scale = readVec(s);

                        obj = new SceneObject(name, id);
                        break;
                    case "PHYSICAL":
                        obj = new SceneObject(name, id);
                        break;
                    case "TERRAIN":
                        position = readVec(s);

                        obj = new SceneObject(name, id);
                        break;
                    case "WATER":
                        position = readVec(s);
                        scale = readVec(s);

                        obj = new SceneObject(name, id);
                        break;
                    case "CAMERA":
                        double clip_dist = s.nextDouble();
                        double speed = s.nextDouble();
                        obj = new SceneObject(name, id);
                        break;
                    case "DIRECTIONAL_LIGHT":
                        color = readVec(s);
                        Vec3 direction = readVec(s);

                        obj = new SceneObject(name, id);
                        break;
                    case "POINT_LIGHT":
                        color = readVec(s);
                        position = readVec(s);
                        double radius = s.nextDouble();
                        Vec3 attenuation = readVec(s);

                        obj = new SceneObject(name, id);
                        break;
                    case "AUDIO_SOURCE":
                        position = readVec(s);

                        obj = new SceneObject(name, id);
                        break;
                    case "NUMBER_VALUE":
                        double val_num = s.nextDouble();

                        obj = new SceneObject(name, id);
                        break;
                    case "VECTOR_VALUE":
                        Vec3 val_vec = readVec(s);

                        obj = new SceneObject(name, id);
                        break;
                    case "SCRIPT":
                        path = readString(s);

                        obj = new SceneObject(name, id);
                        break;
                }

                components[parent].addChild(obj);
                components.Add(id, obj);
            }

            sceneTree.Nodes.Clear();
            sceneTree.Nodes.Add(root.MakeNode());
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
    }

    public class Vec3
    {
        public double x, y, z;

        public Vec3(double x, double y, double z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }
    }

    public class SceneObject
    {
        private string name;
        private int id;
        private SceneObject parent;
        private List<SceneObject> children = new List<SceneObject>();

        public SceneObject(string name, int id)
        {
            this.name = name;
            this.id = id;
        }

        public void removeChild(SceneObject child)
        {
            children.Remove(child);
            child.parent = null;
        }

        public void addChild(SceneObject child)
        {
            children.Add(child);
            child.parent = this;
        }

        public override string ToString()
        {
            return name;
        }

        public TreeNode MakeNode()
        {
            TreeNode node = new TreeNode();
            node.Text = name;
            node.Tag = this;
            foreach(SceneObject child in children)
            {
                node.Nodes.Add(child.MakeNode());
            }
            return node;
        }
    }
}
