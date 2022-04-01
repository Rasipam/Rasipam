export default function getJsonValFromDotChain(obj, dotChain) {
    const keys = dotChain.split('.');
    let iter = obj;
    try {
        keys.forEach(key => iter = iter[key]);
    } catch (e) {
        throw Error('Cannot access the key!');
    }
    return iter;
}
