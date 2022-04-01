function findPos(num, arr) {
    for (let i = 0; i < arr.length; i++)
        if (num < arr[i]) return i - 1;
    return arr.length - 1;
}

const scale = (sourceScale, targetScale) => {
    if (sourceScale.length !== targetScale.length) throw Error('Mismatch scale length');
    const step = targetScale.map((n, i, a) => (a[i + 1] - n) / (sourceScale[i + 1] - sourceScale[i]));
    return num => {
        if (num < sourceScale[0]) return targetScale[0];
        if (num >= sourceScale[sourceScale.length - 1]) return targetScale[targetScale.length - 1];

        const i = findPos(num, sourceScale);
        return targetScale[i] + step[i] * (num - sourceScale[i]);
    }
}

export default scale;
