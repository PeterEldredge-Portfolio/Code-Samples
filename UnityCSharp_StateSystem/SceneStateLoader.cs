// Another simple example of the Event System in use with the State Manager.

using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class SceneStateLoader : GameEventUserObject
{
    [SerializeField] private List<int> _sceneIndexes;

    private int _currentIndex = -1;

    private void Start()
    {
        OnStateChanged(new Events.StateChangedEventArgs(StateManager.Instance.CurrentState));
    }

    private void OnStateChanged(Events.StateChangedEventArgs args)
    {
        if(_currentIndex >= 0)
        {
            AsyncOperation unloadOperation = SceneManager.UnloadSceneAsync(_currentIndex);

            unloadOperation.completed += (AsyncOperation operation) => LoadScene(args.CurrentState);
        }
        else
        {
            LoadScene(args.CurrentState);
        }
    }

    private void LoadScene(int currentState)
    {
        _currentIndex = _sceneIndexes[currentState];

        AsyncOperation loadOperation = SceneManager.LoadSceneAsync(_currentIndex, LoadSceneMode.Additive);

        loadOperation.completed += (AsyncOperation operation) => LightProbes.TetrahedralizeAsync();
    }

    public override void Subscribe()
    {
        EventManager.Instance.AddListener<Events.StateChangedEventArgs>(this, OnStateChanged);
    }

    public override void Unsubscribe()
    {
        EventManager.Instance.RemoveListener<Events.StateChangedEventArgs>(this, OnStateChanged);
    }
}
