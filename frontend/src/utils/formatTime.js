const months = [
    'Jan.', 'Feb.', 'Mar.', 'Apr.', 'May', 'June', 'July', 'Aug.', 'Sep.', 'Oct.', 'Nov.', 'Dec.'
];

export default function formatTime(date) {
    const currentTime = new Date();
    const curDay = currentTime.getDate();
    let [day, hour, min, sec] = [date.getDate(), date.getHours(), date.getMinutes(), date.getSeconds()];

    hour = hour.toString().padStart(2, '0');
    min = min.toString().padStart(2, '0');
    sec = sec.toString().padStart(2, '0');


    return `${hour}:${min}`;

    // const time = `${hour}:${min}.${sec}`;
    // if (curDay === day)
    //     return time;
    //
    // const month = date.getMonth();
    // return `${months[month]} ${day} ` + time;
}
