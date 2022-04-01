import {useEffect, useState} from "react";

export default function useMouseOffset(ref) {
    const [pos, setPos] = useState({x: null, y: null});

    useEffect(() => {
        if (!ref.current) return;
        ref.current.onmousemove = e => {
            const box = ref.current.getBoundingClientRect();
            setPos({
                x: e.clientX - box.x,
                y: e.clientY - box.y
            });
        }
        ref.current.onmouseleave = e => {
            setPos({x: null, y: null});
        }
    }, []);

    return pos;
}
