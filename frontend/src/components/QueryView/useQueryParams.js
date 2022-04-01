import {useCallback, useState} from "react";

export const types = [
    {
        objective: 'TacticSet',
        types: [
            {
                type: 'LimitIndex',
                params: ['min', 'max'],
            },
            {
                type: 'LimitLength',
                params: ['min', 'max', 'offset'],
            },
        ]
    },
    {
        objective: 'Attribute',
        types: [
            {
                type: 'SetExistence',
                params: ['attr', 'exist'],
            },
            {
                type: 'SetImportance',
                params: ['attr', 'importance'],
            },
        ]
    },
    {
        objective: 'Tactic',
        types: [
            {
                type: 'Delete',
                params: ['index'],
            },
            {
                type: 'Split',
                params: ['index', 'attr', 'hit'],
            },
            {
                type: 'Merge',
                params: ['index'],
            },
        ]
    },
    {
        objective: 'Hit',
        types: [
            {
                type: 'Increment',
                params: ['index', 'direction', 'hitCount'],
            },
            {
                type: 'Decrement',
                params: ['index', 'direction', 'hitCount'],
            },
        ]
    },
    {
        objective: 'Value',
        types: [
            {
                type: 'Replace',
                params: ['index', 'hit', 'attr', 'target'],
            },
            {
                type: 'Ignore',
                params: ['index', 'hit', 'attr'],
            },
            {
                type: 'Explore',
                params: ['index', 'hit', 'attr'],
            },
        ]
    }
]

export function findQueryType(type) {
    for (const tc of types)
        for (const t of tc.types)
            if (t.type === type)
                return t;
    return null;
}

export const useQueryParams = () => {
    const [queryParams, setQueryParams] = useState({
        text: '',
        type: null,
        params: null,
    })
    const clearQueryParams = useCallback(() => setQueryParams({
        text: '',
        type: null,
        params: null,
    }), []);
    const validateSetQueryParams = useCallback(newOnes =>
        setQueryParams(oldOnes => ({
            ...oldOnes,
            ...newOnes,
        })), []);
    const isApplicable = () => {
        return queryParams.type !== null;
    }
    return {
        queryParams,
        setQueryParams: validateSetQueryParams,
        clearQueryParams,
        isApplicable,
    }
}
