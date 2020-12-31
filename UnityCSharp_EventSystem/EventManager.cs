// This is a Singleton Event Manager that I created as a tool for use in most of my projects.
// My goal was to make something that was efficient, could keep indepentent systems seperated, and was simple to use.
// It manages a Data Structure that is a dictionary of actions corresponding to specific events.
// Events in this system are structs which implement the IGameEvent Interface. (It is an empty interface used for organization purposes)
// Any object may listen for an event, and any object may trigger an event.
// When an object listens for an event, it is notified whenever the event is called, and the struct is passed as a parameter.
// There is a limit to the number of Events that may be triggered per frame. (_MAX_TRIGGERED_EVENTS_PER_FRAME)
// Important events for things like input, may override this using the TriggerEventImmeditate Method, as oppoesed to TriggerEvent.
// Generally, I do not use tons of comments in my code.
// I try to make variable and method names as specific as possible.
// However, for my portfolio I am adding in additional comments.
// I would strongly recommend looking at the Example scripts for a simple use case.

using System;
using System.Linq;
using System.Collections.Generic;
using UnityEngine;

// A simple struct which holds an Object along with the action it would like to perform.
// Used to track an object for when it wants to stop listening for an event.
public struct ObjectActionPair<T>
{
    public object Object { get; }
    public Action<T> Action { get; }

    public ObjectActionPair(object obj, Action<T> action)
    {
        Object = obj;
        Action = action;
    }
}

public class EventManager : MonoBehaviour
{
    private const int _MAX_TRIGGERED_EVENTS_PER_FRAME = 5;

    public static EventManager Instance { get; private set; }

    // The primary data strure that stores all events and the objects/actions listening for those events.
    public Dictionary<Type, List<ObjectActionPair<IGameEvent>>> _eventListeners = new Dictionary<Type, List<ObjectActionPair<IGameEvent>>>();

    // The queue of events that need to be triggered.
    private Queue<IGameEvent> _triggeredEventQueue = new Queue<IGameEvent>();

    // The remaining events left to be triggered this turn.
    private int _eventsLeftToTrigger = 0;

    // Storage for an event type.
    // This is here so that the variable does need need to be created whenever an event type needs to be stored in a method.
    private Type _eventTypeStorage;

    // Initializes the singleton, this needs to be done before other objects excecute their Awake methods.
    // For this reason, this script must happen first in the Script Excecution Order
    private void Awake()
    {
        if (Instance == null) Instance = this;
        else Destroy(this);
    }

    private void LateUpdate()
    {
        while(_eventsLeftToTrigger > 0 && _triggeredEventQueue.Count > 0)
        {
            TriggerEventImmediate(_triggeredEventQueue.Dequeue());
        }

        _eventsLeftToTrigger = _MAX_TRIGGERED_EVENTS_PER_FRAME;
    }

    // Adds a new Object Action Pair to the dictionary for each object that wants to listen for the event.
    public void AddListener<T>(object obj, Action<T> action) where T : IGameEvent
    {
        _eventTypeStorage = typeof(T);

        if (!_eventListeners.ContainsKey(_eventTypeStorage))
        {
            _eventListeners.Add(_eventTypeStorage, new List<ObjectActionPair<IGameEvent>>());
        }

        _eventListeners[_eventTypeStorage].Add(new ObjectActionPair<IGameEvent>(obj, new Action<IGameEvent>((eventArgs) => action.Invoke((T)eventArgs))));
    }

    public void RemoveListener<T>(object obj, Action<T> action) where T : IGameEvent
    {
        _eventTypeStorage = typeof(T);

        if (_eventListeners != null && _eventListeners.ContainsKey(_eventTypeStorage))
        {
            _eventListeners[_eventTypeStorage] = _eventListeners[_eventTypeStorage].OfType<ObjectActionPair<IGameEvent>>().Where(x => x.Object != obj).ToList();
        }
    }

    public void TriggerEvent(IGameEvent gameEventArgs)
    {
        _triggeredEventQueue.Enqueue(gameEventArgs);
    }

    // Simply loops through the list of actions corresponding to an event, invoking all actions in the list.
    public void TriggerEventImmediate(IGameEvent gameEventArgs)
    {
        _eventTypeStorage = gameEventArgs.GetType();

        if (_eventListeners.ContainsKey(_eventTypeStorage))
        {
            for (int i = 0; i < _eventListeners[_eventTypeStorage].Count; i++)
            {
                _eventListeners[_eventTypeStorage][i].Action.Invoke(gameEventArgs);
            }
        }

        _eventsLeftToTrigger--;
    }
}
