// This has been condensed into one script to make it simpler to view.

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// All events are put into the Events namespace for easy access (and to avoid conflicts).
namespace Events
{
    // Events can be empty, in that case they are simply messages.
    public struct GameStartedEventArgsExample : IGameEvent { }

    // Or they may contain important state information.
    public struct GameOverEventArgsExample : IGameEvent
    {
        public float Time { get; }

        public GameOverEventArgsExample(float time)
        {
            Time = time;
        }
    }
}

// This is an example of an object that listens for an event.
public class EventExampleObj : GameEventUserObject
{
    // To listen for an event, you must have a method that takes in the event struct as an argument.
    private void DisplayFinalScore(Events.GameOverEventArgsExample eventArgs)
    {
        Debug.Log("Display " + eventArgs.Time.ToString());
    }

    // Listening for the event is as simple as calling AddListener with the method you would like to be called.
    public override void Subscribe()
    {
        EventManager.Instance.AddListener<Events.GameOverEventArgsExample>(this, DisplayFinalScore);
    }

    public override void Unsubscribe()
    {
        EventManager.Instance.RemoveListener<Events.GameOverEventArgsExample>(this, DisplayFinalScore);
    }
}

// This is an example of an object that triggers an event.
public class EventExampleTriggererObj : MonoBehaviour
{
    private void Awake()
    {
        StartCoroutine(EndGame(5f));
    }

    private IEnumerator EndGame(float seconds)
    {
        yield return new WaitForSeconds(seconds);

        EventManager.Instance.TriggerEvent(new Events.GameOverEventArgsExample(seconds));
    }
}