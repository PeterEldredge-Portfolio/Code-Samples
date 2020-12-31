//Condensed to one script for ease of reading.

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct LightingStateData
{
    public string SkyboxName;
    public Color FogColor;
    public float FogDensity;
}

public class LightingGlobalStateUpdater : GlobalStateUpdater<LightingStateData>
{
    protected override void UpdateState()
    {
        if (string.IsNullOrEmpty(_stateDatas[CurrentState].SkyboxName)) return;

        RenderSettings.skybox = (Material)Resources.Load(_stateDatas[CurrentState].SkyboxName);

        RenderSettings.fogColor = _stateDatas[CurrentState].FogColor;
        RenderSettings.fogDensity = _stateDatas[CurrentState].FogDensity;

        DynamicGI.UpdateEnvironment();
    }
}