// This abstract class serves as a template for objects which want to update when the state is changed.
// A data class/struct is created with the necessary information to update.
// Then you can simply override the UpdateState method with whatever functionality you want.
// LightingGlobalStateUpdater, ObjectGlobalStateUpdater, and PostProcessGlobalStateUpdater all are subclasses.

using UnityEngine;

public abstract class GlobalStateUpdater<T> : GameEventUserObject
{
    [SerializeField] protected T[] _stateDatas = new T[StateManager.NUMBER_OF_STATES];

    private int _currentState;
    public int CurrentState
    {
        get { return _currentState; }
        protected set
        {
            _currentState = value;
            UpdateState();
        }
    }

    protected virtual void SetCurrentState(Events.StateChangedEventArgs eventArgs)
    {
        CurrentState = eventArgs.CurrentState;
    }

    protected override void Subscribe()
    {
        EventManager.Instance.AddListener<Events.StateChangedEventArgs>(this, SetCurrentState);
    }

    protected override void Unsubscribe()
    {
        EventManager.Instance.RemoveListener<Events.StateChangedEventArgs>(this, SetCurrentState);
    }

    protected void Start()
    {
        CurrentState = StateManager.Instance.CurrentState;
    }

    protected virtual void UpdateState() { }
}
