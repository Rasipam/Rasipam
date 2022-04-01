import randomInt from "./randomInt";

export default function uuid4() {
    let d = new Date().getTime();
    return 'aaaaaaaa-aaaa-4aaa-baaa-aaaaaaaaaaaa'.replace(/[ab]/g, c => {
        let r = randomInt(16);
        r = (d + r) % 16 | 0;
        d = Math.floor(d / 16);
        const digit = c === 'a' ?
            r :
            ((r & 0b11) | 0b1000)
        return digit.toString(16);
    });
}
