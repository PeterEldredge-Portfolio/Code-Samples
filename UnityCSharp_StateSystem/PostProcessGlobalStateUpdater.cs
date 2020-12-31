//Condensed to one script for ease of reading.

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering.PostProcessing;

[System.Serializable]
public struct PostProcessStateData
{
    public PostProcessProfile Profile;
}

public class PostProcessGlobalStateUpdater : GlobalStateUpdater<PostProcessStateData>
{
    private PostProcessVolume _volume;

    private void Awake()
    {
        _volume = GetComponent<PostProcessVolume>();
    }

    protected override void UpdateState()
    {
        _volume.sharedProfile = _stateDatas[CurrentState].Profile;
    }
}
