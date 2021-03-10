using System.Collections;
using System.Collections.Generic;
using System.Threading.Tasks;
using Peace.Serialization;
using UnityEditor;
using UnityEngine;

namespace Peace
{

    [CustomPropertyDrawer(typeof(TerrainSystem.Layer))]
    public class TerrainSystemLayerDrawer : PropertyDrawer
    {
        const float ROW_SPACING = 5;
        const float END_SPACING = 30;

        public override void OnGUI(Rect position, SerializedProperty property, GUIContent label)
        {
            EditorGUI.BeginProperty(position, label, property);
            float hu = EditorGUI.GetPropertyHeight(property.FindPropertyRelative("name"));
            float rh = hu + ROW_SPACING;
            Vector2 o = position.position;
            float w = position.width;

            int row = 0;

            EditorGUI.PropertyField(new Rect(o.x, o.y + row * rh, w, hu), property.FindPropertyRelative("name"), new GUIContent("name"));
            ++row;
            EditorGUI.PropertyField(new Rect(o.x, o.y + row * rh, w, hu), property.FindPropertyRelative("color"), new GUIContent("color"));
            ++row;

            GUIStyle titleStyle = new GUIStyle(GUI.skin.label);
            titleStyle.fontStyle = FontStyle.Bold;

            EditorGUI.LabelField(new Rect(o.x, o.y + row * rh, w, hu), new GUIContent("Distribution w.r.t altitude"), titleStyle);
            ++row;

            float oldLblWidth = EditorGUIUtility.labelWidth;
            EditorGUIUtility.labelWidth = 50;

            SerializedProperty distribParams = property.FindPropertyRelative("distribParams");

            EditorGUI.PropertyField(new Rect(o.x, o.y + row * rh, 0.5f * w, hu), distribParams.FindPropertyRelative("hmin"), new GUIContent("min: "));
            EditorGUI.PropertyField(new Rect(o.x + 0.5f * w, o.y + row * rh, 0.5f * w, hu), distribParams.FindPropertyRelative("hmax"), new GUIContent("max: "));
            ++row;
            EditorGUI.PropertyField(new Rect(o.x, o.y + row * rh, 0.25f * w, hu), distribParams.FindPropertyRelative("ha"), GUIContent.none);
            EditorGUI.PropertyField(new Rect(o.x + 0.25f * w, o.y + row * rh, 0.25f * w, hu), distribParams.FindPropertyRelative("hb"), GUIContent.none);
            EditorGUI.PropertyField(new Rect(o.x + 0.5f * w, o.y + row * rh, 0.25f * w, hu), distribParams.FindPropertyRelative("hc"), GUIContent.none);
            EditorGUI.PropertyField(new Rect(o.x + 0.75f * w, o.y + row * rh, 0.25f * w, hu), distribParams.FindPropertyRelative("hd"), GUIContent.none);
            ++row;

            AnimationCurve heightCurve = new AnimationCurve();
            float hmin = distribParams.FindPropertyRelative("hmin").floatValue;
            float hmax = distribParams.FindPropertyRelative("hmax").floatValue;
            heightCurve.AddKey(distribParams.FindPropertyRelative("ha").floatValue, hmin);
            heightCurve.AddKey(distribParams.FindPropertyRelative("hb").floatValue, hmax);
            heightCurve.AddKey(distribParams.FindPropertyRelative("hc").floatValue, hmax);
            heightCurve.AddKey(distribParams.FindPropertyRelative("hd").floatValue, hmin);
            prepareCurve(heightCurve);

            EditorGUI.CurveField(new Rect(o.x, o.y + row * rh, w, 2 * hu), heightCurve);
            row += 2;
            
            EditorGUI.LabelField(new Rect(o.x, o.y + row * rh, w, hu), new GUIContent("Distribution w.r.t slope"), titleStyle);
            ++row;

            EditorGUI.PropertyField(new Rect(o.x, o.y + row * rh, 0.5f * w, hu), distribParams.FindPropertyRelative("dhmin"), new GUIContent("min: "));
            EditorGUI.PropertyField(new Rect(o.x + 0.5f * w, o.y + row * rh, 0.5f * w, hu), distribParams.FindPropertyRelative("dhmax"), new GUIContent("max: "));
            ++row;
            EditorGUI.PropertyField(new Rect(o.x, o.y + row * rh, 0.25f * w, hu), distribParams.FindPropertyRelative("dha"), GUIContent.none);
            EditorGUI.PropertyField(new Rect(o.x + 0.25f * w, o.y + row * rh, 0.25f * w, hu), distribParams.FindPropertyRelative("dhb"), GUIContent.none);
            EditorGUI.PropertyField(new Rect(o.x + 0.5f * w, o.y + row * rh, 0.25f * w, hu), distribParams.FindPropertyRelative("dhc"), GUIContent.none);
            EditorGUI.PropertyField(new Rect(o.x + 0.75f * w, o.y + row * rh, 0.25f * w, hu), distribParams.FindPropertyRelative("dhd"), GUIContent.none);
            ++row;

            AnimationCurve slopeCurve = new AnimationCurve();
            float dhmin = distribParams.FindPropertyRelative("dhmin").floatValue;
            float dhmax = distribParams.FindPropertyRelative("dhmax").floatValue;
            slopeCurve.AddKey(distribParams.FindPropertyRelative("dha").floatValue, dhmin);
            slopeCurve.AddKey(distribParams.FindPropertyRelative("dhb").floatValue, dhmax);
            slopeCurve.AddKey(distribParams.FindPropertyRelative("dhc").floatValue, dhmax);
            slopeCurve.AddKey(distribParams.FindPropertyRelative("dhd").floatValue, dhmin);
            prepareCurve(slopeCurve);

            EditorGUI.CurveField(new Rect(o.x, o.y + row * rh, w, 2 * hu), slopeCurve);
            row += 2;

            if (GUI.Button(new Rect(o.x, o.y + row * rh, w * 0.25f, hu), "Move Before"))
            {
                property.FindPropertyRelative("posChange").intValue = -1;
            }
            if (GUI.Button(new Rect(o.x + w * 0.25f, o.y + row * rh, w * 0.75f, hu), "Remove"))
            {
                property.FindPropertyRelative("toRemove").boolValue = true;
            }
            ++row;
            if (GUI.Button(new Rect(o.x, o.y + row * rh, w * 0.25f, hu), "Move After"))
            {
                property.FindPropertyRelative("posChange").intValue = 1;
            }
            if (GUI.Button(new Rect(o.x + w * 0.25f, o.y + row * rh, w * 0.75f, hu), "Add Layer After"))
            {
                property.FindPropertyRelative("addAfter").boolValue = true;
            }

            ++row;

            EditorGUIUtility.labelWidth = oldLblWidth;
            EditorGUI.EndProperty();
        }

        public override float GetPropertyHeight(SerializedProperty property, GUIContent label)
        {
            float rh = EditorGUI.GetPropertyHeight(property.FindPropertyRelative("name")) + ROW_SPACING;
            return rh * 14 + END_SPACING;
        }

        private void prepareCurve(AnimationCurve curve)
        {
            SetCurveTangentLinear(curve);

            curve.AddKey(new Keyframe(0.01f, curve.Evaluate(0)));
            curve.AddKey(new Keyframe(0.99f, curve.Evaluate(1)));
            curve.AddKey(new Keyframe(0, 0));
            curve.AddKey(new Keyframe(1, 1));

            for (int i = curve.length - 1; i >= 0; --i)
            {
                if (curve[i].time > 1 || curve[i].time < 0)
                {
                    curve.RemoveKey(i);
                    continue;
                }
            }

            SetCurveTangentLinear(curve);
        }

        private void SetCurveTangentLinear(AnimationCurve curve)
        {
            for (int i = 0; i < curve.length; ++i)
            {
                //AnimationUtility.SetKeyBroken(curve, i, true);
                Keyframe kf = curve.keys[i];
                if (i != 0)
                {
                    kf.inTangent = curve[i - 1].outTangent;
                }

                if (i != curve.length - 1)
                {
                    kf.outTangent =
                        (curve[i + 1].value - curve[i].value) / (curve[i + 1].time - curve[i].time);
                }
                curve.MoveKey(i, kf);
            }
        }
    }

    [CustomEditor(typeof(TerrainSystem))]
    public class TerrainSystemEditor : Editor
    {
        [MenuItem("GameObject/World/Terrain System", false, 0)]
        public static void CreateTerrainSystem()
        {
            GameObject terrainSystemGO = new GameObject("Terrain System");
            var terrainSystem = terrainSystemGO.AddComponent<TerrainSystem>();
            terrainSystem.materialTemplate = new Material(Shader.Find("Standard"));
            terrainSystem.SetupDefaultLayers();
            Undo.RegisterCreatedObjectUndo(terrainSystemGO, "Created Terrain System");
        }


        private TerrainSystem _terrainSystem;

        void OnEnable()
        {
            _terrainSystem = (TerrainSystem) target;
        }

        public override void OnInspectorGUI()
        {
            base.OnInspectorGUI();

            // Remove layers if needed
            for (int i = _terrainSystem.layers.Count - 1; i >= 0; --i)
            {
                if (_terrainSystem.layers[i].toRemove)
                {
                    _terrainSystem.layers.RemoveAt(i);
                }
                else if (_terrainSystem.layers[i].addAfter)
                {
                    _terrainSystem.layers.Insert(i + 1, new TerrainSystem.Layer());
                    var layer = _terrainSystem.layers[i];
                    layer.addAfter = false;
                    _terrainSystem.layers[i] = layer;
                }
                else if (_terrainSystem.layers[i].posChange != 0)
                {
                    int targPos = i + _terrainSystem.layers[i].posChange;

                    if (targPos >= 0 && targPos < _terrainSystem.layers.Count)
                    {
                        var layer = _terrainSystem.layers[i];
                        layer.posChange = 0;
                        _terrainSystem.layers[i] = _terrainSystem.layers[targPos];
                        _terrainSystem.layers[targPos] = layer;
                    }
                }
            }

            string buttonText = _terrainSystem.IsEmpty() ? "Generate" : "Regenerate";

            GUI.enabled = !_terrainSystem.generating;
            if (GUILayout.Button(buttonText))
            {
                _terrainSystem.Regenerate();
            }
            GUI.enabled = true;
        }

        private void AddTestTerrain()
        {
            GameObject terrainGO = new GameObject("Terrain");
            terrainGO.transform.SetParent(_terrainSystem.transform);
            var terrain = terrainGO.AddComponent<Terrain>();

            var tData = new TerrainData();
            tData.heightmapResolution = 255;
            tData.size = new Vector3(20, 20, 20);

            terrain.terrainData = tData;
            terrainGO.AddComponent<TerrainCollider>().terrainData = terrain.terrainData;
        }

        public void OnSceneGUI()
        {
            float tw = _terrainSystem.tileWidth;
            var extensions = _terrainSystem.GetExtensionPossibility();

            List<Vector2Int> generate = new List<Vector2Int>();

            foreach (var extPos in extensions)
            {
                Vector3 buttonLoc = new Vector3((extPos.x + 0.5f) * tw, _terrainSystem.minAltitude, (extPos.y + 0.5f) * tw);
                float pickSize = _terrainSystem.generating ? 0 : tw / 2;

                if (Handles.Button(buttonLoc, Quaternion.LookRotation(Vector3.up), tw / 2, pickSize, Handles.RectangleHandleCap))
                {
                    generate.Add(extPos);
                }
            }

            if (!_terrainSystem.generating)
            { 
                foreach (Vector2Int pos in generate)
                {
                    _terrainSystem.GenerateTile(pos);
                }
            }
            else
            {
                foreach (Vector2Int pos in _terrainSystem.generatingCoords)
                {
                    DrawHandleBox(
                        new Vector3(pos.x * tw, _terrainSystem.minAltitude, pos.y * tw),
                        new Vector3(tw, _terrainSystem.maxAltitude - _terrainSystem.minAltitude, tw),
                        new Color(0, 0.8f, 0.9f, 0.5f)
                    );
                }
            }
        }

        private void DrawHandleBox(Vector3 o, Vector3 sizes, Color color)
        {
            Vector3 sx = Vector3.right * sizes.x;
            Vector3 sy = Vector3.up * sizes.y;
            Vector3 sz = Vector3.forward * sizes.z;

            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o, o + sx, o + sx + sy, o + sy }, color, color);
            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o, o + sz, o + sz + sy, o + sy }, color, color);
            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o, o + sx, o + sx + sz, o + sz }, color, color);
            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o + sx, o + sx + sy, o + sx + sy + sz, o + sx + sz }, color, color);
            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o + sy, o + sx + sy, o + sx + sy + sz, o + sy + sz }, color, color);
            Handles.DrawSolidRectangleWithOutline(new Vector3[] { o + sz, o + sx + sz, o + sx + sy + sz, o + sy + sz }, color, color);
        }
    }
}

