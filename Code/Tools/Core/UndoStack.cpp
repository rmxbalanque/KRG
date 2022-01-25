#include "UndoStack.h"

//-------------------------------------------------------------------------

namespace KRG
{
    UndoStack::~UndoStack()
    {
        ClearRedoStack();
        ClearUndoStack();
    }

    //-------------------------------------------------------------------------

    IUndoableAction const* UndoStack::Undo()
    {
        KRG_ASSERT( CanUndo() );

        // Pop action off the recorded stack
        IUndoableAction* pAction = m_recordedActions.back();
        m_recordedActions.pop_back();

        // Undo the action and place it on the undone stack
        pAction->Undo();
        m_undoneActions.emplace_back( pAction );
        return pAction;
    }

    IUndoableAction const* UndoStack::Redo()
    {
        KRG_ASSERT( CanRedo() );

        // Pop action off the recorded stack
        IUndoableAction* pAction = m_undoneActions.back();
        m_undoneActions.pop_back();

        // Undo the action and place it on the undone stack
        pAction->Redo();
        m_recordedActions.emplace_back( pAction );
        return pAction;
    }

    //-------------------------------------------------------------------------

    void UndoStack::RegisterAction( IUndoableAction* pAction )
    {
        KRG_ASSERT( pAction != nullptr );
        m_recordedActions.emplace_back( pAction );
        ClearRedoStack();
    }

    void UndoStack::ClearUndoStack()
    {
        for ( auto& pAction : m_recordedActions )
        {
            KRG::Delete( pAction );
        }

        m_recordedActions.clear();
    }

    void UndoStack::ClearRedoStack()
    {
        for ( auto& pAction : m_undoneActions )
        {
            KRG::Delete( pAction );
        }

        m_undoneActions.clear();
    }

}