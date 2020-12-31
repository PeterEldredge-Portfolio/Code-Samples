// This is a simple Singleton State Manager which tracks the Current State.
// When the state changes, the StateChangedEventArgs Event is triggered.

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StateManager : MonoBehaviour
{
    public static StateManager Instance { get; private set; }

    public const int NUMBER_OF_STATES = 4;

    public int CurrentState { get; private set; } = 0;

    private void Awake()
    {
        if (Instance == null) Instance = this;
        else Destroy(this);
    }

    [ContextMenu("Trigger State Change")]
    public void TriggerStateChange()
    {
        CurrentState = (CurrentState + 1) % NUMBER_OF_STATES;

        EventManager.Instance.TriggerEvent(new Events.StateChangedEventArgs(CurrentState));
    }

    [ContextMenu("Reset State")]
    public void ResetState()
    {
        CurrentState = 0;

        EventManager.Instance.TriggerEvent(new Events.StateChangedEventArgs(CurrentState));
    }
}
