using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// This is a simple class created to reduce unecessary code to use the Event System.
// Simply inherit from GameEventUserObject instead of MonoBehaviour to have the Subscribe and Unsubscribe functions set up for you.
// By listening in Subscribe and removing the listener in Unsubscribe, you are ensuring that whenever the object is active, it will hear the event.
public abstract class GameEventUserObject : MonoBehaviour, IUseGameEvents
{
    protected virtual void OnEnable()
    {
        Subscribe();
    }

    protected virtual void OnDisable()
    {
        Unsubscribe();
    }

    public virtual void Subscribe() {}
    public virtual void Unsubscribe() {}
}
