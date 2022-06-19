using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ThreeD_SceneBuilder
{
    public class SceneObject
    {
        public string name;
        public int id;
        public SceneObject parent;
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
            foreach (SceneObject child in children)
            {
                node.Nodes.Add(child.MakeNode());
            }
            return node;
        }

        public void getChildrenRecursive(List<SceneObject> objects)
        {
            objects.Add(this);
            foreach (SceneObject child in children)
            {
                child.getChildrenRecursive(objects);
            }
        }

        public virtual string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 6 OBJECT", id, parent.id, name.Length, name);
        }
    }

    public class MeshObject : SceneObject
    {
        public string path;
        public Vec3 position;
        public Vec3 scale;

        public MeshObject(string name, int id, string path, Vec3 position, Vec3 scale)
            : base(name, id)
        {
            this.path = path;
            this.position = position;
            this.scale = scale;
        }

        public override string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 4 MESH {4} {5} {6} {7}", id, parent.id, name.Length, name, path.Length, path, position, scale);
        }
    }

    public class PhysicalObject : SceneObject
    {

        public PhysicalObject(string name, int id)
            : base(name, id)
        {
        }

        public override string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 8 PHYSICAL", id, parent.id, name.Length, name);
        }
    }

    public class TerrainObject : SceneObject
    {
        public Vec3 position;

        public TerrainObject(string name, int id, Vec3 position)
            : base(name, id)
        {
            this.position = position;
        }

        public override string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 7 TERRAIN {4}", id, parent.id, name.Length, name, position);
        }
    }

    public class WaterObject : SceneObject
    {
        public Vec3 position;
        public Vec3 scale;

        public WaterObject(string name, int id, Vec3 position, Vec3 scale)
            : base(name, id)
        {
            this.position = position;
            this.scale = scale;
        }

        public override string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 5 WATER {4} {5}", id, parent.id, name.Length, name, position, scale);
        }
    }

    public class CameraObject : SceneObject
    {
        public double clip_dist;
        public double speed;

        public CameraObject(string name, int id, double clip_dist, double speed)
            : base(name, id)
        {
            this.clip_dist = clip_dist;
            this.speed = speed;
        }

        public override string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 6 CAMERA {4} {5}", id, parent.id, name.Length, name, clip_dist, speed);
        }
    }

    public class DirectionalLightObject : SceneObject
    {
        public Vec3 color;
        public Vec3 direction;

        public DirectionalLightObject(string name, int id, Vec3 color, Vec3 direction)
            : base(name, id)
        {
            this.color = color;
            this.direction = direction;
        }

        public override string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 17 DIRECTIONAL_LIGHT {4} {5}", id, parent.id, name.Length, name, color, direction);
        }
    }

    public class PointLightObject : SceneObject
    {
        public Vec3 color;
        public Vec3 position;
        public double radius;
        public Vec3 attenuation;

        public PointLightObject(string name, int id, Vec3 color, Vec3 position, double radius, Vec3 attenuation)
            : base(name, id)
        {
            this.color = color;
            this.position = position;
            this.radius = radius;
            this.attenuation = attenuation;
        }

        public override string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 11 POINT_LIGHT {4} {5} {6} {7}", id, parent.id, name.Length, name, color, position, radius, attenuation);
        }
    }

    public class AudioSourceObject : SceneObject
    {
        public Vec3 position;

        public AudioSourceObject(string name, int id, Vec3 position)
            : base(name, id)
        {
            this.position = position;
        }

        public override string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 12 AUDIO_SOURCE {4}", id, parent.id, name.Length, name, position);
        }
    }

    public class NumberValueObject : SceneObject
    {
        public double value;

        public NumberValueObject(string name, int id, double value)
            : base(name, id)
        {
            this.value = value;
        }

        public override string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 12 NUMBER_VALUE {4}", id, parent.id, name.Length, name, value);
        }
    }

    public class Vec3ValueObject : SceneObject
    {
        public Vec3 value;

        public Vec3ValueObject(string name, int id, Vec3 value)
            : base(name, id)
        {
            this.value = value;
        }

        public override string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 12 VECTOR_VALUE {4}", id, parent.id, name.Length, name, value);
        }
    }

    public class ScriptObject : SceneObject
    {
        public string path;

        public ScriptObject(string name, int id, string path)
            : base(name, id)
        {
            this.path = path;
        }

        public override string ToSaveString()
        {
            return string.Format("{0} {1} {2} {3} 6 SCRIPT {4} {5}", id, parent.id, name.Length, name, path.Length, path);
        }
    }
}
