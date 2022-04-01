export default function useRallyHeight(attrCount) {
    const headHeight = 34, attrHeight = 25, m = 4;
    return {
        rallyHeight: expand => headHeight + (expand ? (attrCount * attrHeight + m * 2) : 0),
        headHeight,
        attrHeight,
        m: expand => expand ? m : 0,
    }
}
