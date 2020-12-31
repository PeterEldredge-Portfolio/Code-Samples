//Condensed to one script for ease of reading.

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct ObjectStateData
{
    public GameObject TransformObject;
}

public class ObjectGlobalStateUpdater : GlobalStateUpdater<ObjectStateData>
{
    //Handles missing states in the inspector
    private void Awake()
    {
        for (int i = 0; i < _stateDatas.Length; i++)
        {
            ObjectStateData data = _stateDatas[i];

            if (i == 0)
            {
                if (data.TransformObject == null)
                {
                    GameObject initialStatePositionObject = new GameObject($"{gameObject.name}-InitialStatePosition");

                    initialStatePositionObject.transform.parent = transform.parent;

                    initialStatePositionObject.transform.position = transform.position;
                    initialStatePositionObject.transform.rotation = transform.rotation;
                    initialStatePositionObject.transform.localScale = transform.localScale;

                    _stateDatas[i].TransformObject = initialStatePositionObject;
                }
            }
            else
            {
                if (data.TransformObject == null) _stateDatas[i].TransformObject = _stateDatas[i - 1].TransformObject;
            }
        }
    }

    protected override void UpdateState()
    {
        if (CurrentState > _stateDatas.Length) CurrentState = _stateDatas.Length;

        Transform newStateTransform = _stateDatas[CurrentState - 1].TransformObject.transform;

        transform.position = newStateTransform.position;
        transform.rotation = newStateTransform.rotation;
        transform.localScale = newStateTransform.localScale;
    }

}