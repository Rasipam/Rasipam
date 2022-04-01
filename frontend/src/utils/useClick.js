import {useCallback, useRef} from "react";

export default function useClick({onClick, onDoubleClick}) {
    const interval = useRef(null);

    const handleClick = useCallback((...props) => {
        interval.current = setTimeout(() => {
            if (interval.current === null) return;
            interval.current = null;
            onClick(...props);
        }, 300);
    }, [onClick]);

    const handleDoubleClick = useCallback((...props) => {
        clearTimeout(interval.current);
        interval.current = null;
        onDoubleClick(...props)
    }, [onDoubleClick]);

    return {handleClick, handleDoubleClick}
}
