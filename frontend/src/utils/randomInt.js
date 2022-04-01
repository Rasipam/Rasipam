export default function randomInt(limit1=100, limit2=null) {
    if (limit2 === null) {
        limit2 = limit1;
        limit1 = 0;
    }

    return Math.floor(Math.random() * (limit2 - limit1)) + limit1;
}
