const bs = val => 40 * val;
const r2xy = (r, l) => [l * Math.cos(r), l * Math.sin(r)];

const symbolSets = {
    'Badminton': {
        'Ball Height': {
            pos: (size, asGlyph) => asGlyph ?
                `translate(${0.43 * size[0]}, 0) scale(${size[0] / bs(1)}, ${size[1] / bs(1)})` :
                `scale(${size[0] / bs(1)},${size[1] / bs(1)})`,
            params: {
                'Very low': 0,
                'Low': 1,
                'Medium': 2,
                'High': 3,
            },
            template: valParam => {
                return <g>
                    {[...new Array(4)].map((_, i) => {
                        if (i > valParam) return null;
                        return <rect key={i}
                                     x={-bs(.05)}
                                     y={bs(.25) - i * bs(.25)}
                                     width={bs(.1)}
                                     height={bs(.23)}
                                     strokeWidth={0}
                        />
                    })}
                </g>
            }
        },
        'Ball Position': {
            pos: (size, asGlyph) => asGlyph ?
                `translate(${-0.05 * size[0]}, ${-0.175 * size[1]}) scale(${size[0] * 2 / 3 / bs(1)},${size[1] * 2 / 3 / bs(1)})` :
                `scale(${size[0] / bs(1)},${size[1] / bs(1)})`,
            params: {
                'Backcourt Left': 4,
                'Backcourt Right': 5,
                'Midfield Left': 2,
                'Midfield Right': 3,
                'Forecourt Left': 0,
                'Forecourt Right': 1,
            },
            template: valParam => {
                const fullLen = 3.2;
                const vectors = [
                    [[-bs(.3), bs(.5)], [-bs(.5), -bs(.3)]],
                    [[0, bs(.45)], [0, -bs(.5)]],
                    [[bs(.3), bs(.5)], [bs(.5), -bs(.3)]],
                ]
                const vector = (idx, len, xy) => vectors[idx][1][xy] * len / fullLen + vectors[idx][0][xy] * (1 - len / fullLen);
                return <g>
                    {[...new Array(6)].map((_, i) => {
                        const x = i % 2, y = Math.floor(i / 2);
                        return <path key={i}
                                     d={`M${vector(x, y, 0)} ${vector(x, y, 1)}L${vector(x + 1, y, 0)} ${vector(x + 1, y, 1)}L${vector(x + 1, y + 1, 0)} ${vector(x + 1, y + 1, 1)}L${vector(x, y + 1, 0)} ${vector(x, y + 1, 1)}Z`}
                                     strokeWidth={1}
                                     fill={valParam === i ? undefined : '#fff'}/>
                    })}
                    {[...new Array(3)].map((_, i) => (
                        <line key={i}
                              x1={vector(i, 0, 0)} y1={vector(i, 0, 1)}
                              x2={vector(i, fullLen, 0)} y2={vector(i, fullLen, 1)}
                              strokeWidth={1}/>
                    ))}
                </g>
            }
        },
        'Hit Technique': {
            pos: (size, asGlyph) => asGlyph ?
                `translate(${-0.05 * size[0]},${0.325 * size[1]}) scale(${size[0] * 0.4 / bs(1)},${size[1] * 0.4 / bs(1)})` :
                `scale(${size[0] / bs(1)},${size[1] / bs(1)})`,
            params: {
                'Net service': 'NS',
                'Backcourt service': 'BS',
                'Smash': 'Sma.',
                'Drive': 'Dri.',
                'Lift': 'Lift',
                'High clear': 'HClr',
                'Hook': 'Hook',
                'Shot': 'Shot',
                'Net shot': 'NShot',
                'Drop': 'Drop',
                'Push': 'Push',
                'Block': 'Blk',
                'Other': 'Oth.',
                'Score': '+1',
            },
            template: valParam => {
                return <g>
                    <path
                        d={`M-${bs(.5)} -${bs(.35)}H${bs(.5)}V-${bs(.15)}A${bs(.5)} ${bs(.5)} 0 0 1 -${bs(.5)} -${bs(.15)}Z`}
                        fillOpacity={0} strokeWidth={1}/>
                    <text x={0} y={-bs(0.1)} textAnchor={'middle'} dominantBaseline={'middle'}
                          strokeWidth={0}
                          fontSize={bs(.3)}>{valParam}</text>
                </g>
            }
        }
    },
    'Tennis': {
        'Ball Position': {
            pos: (size, asGlyph) => asGlyph ?
                `translate(0, ${-0.14 * size[1]}) scale(${size[0] * 0.55 / bs(1)},${size[1] * 0.55 / bs(1)})` :
                `scale(${size[0] / bs(1)},${size[1] / bs(1)})`,
            params: {
                'Starting Tee 1': null,
                'Starting Tee 2': null,
                'Starting Tee 3': null,
                'Starting Tee 4': null,
                'Serving right zone outside corner': [0, 0],
                'Serving right zone midway': [0, 1],
                'Serving right zone inside corner': [0, 2],
                'Serving left zone outside corner': [0, 5],
                'Serving left zone midway': [0, 4],
                'Serving left zone inside corner': [0, 3],
                'Right zone front': [1, [0, 3]],
                'Right zone midfront': [1, [0, 2]],
                'Right zone midback': [1, [0, 1]],
                'Right zone back': [1, [0, 0]],
                'Midright zone front': [1, [1, 3]],
                'Midright zone midfront': [1, [1, 2]],
                'Midright zone midback': [1, [1, 1]],
                'Midright zone back': [1, [1, 0]],
                'Midleft zone front': [1, [2, 3]],
                'Midleft zone midfront': [1, [2, 2]],
                'Midleft zone midback': [1, [2, 1]],
                'Midleft zone back': [1, [2, 0]],
                'Left zone front': [1, [3, 3]],
                'Left zone midfront': [1, [3, 2]],
                'Left zone midback': [1, [3, 1]],
                'Left zone back': [1, [3, 0]],
                'RF RMF MRF MRMF no drop point': [2, [0, 0]],
                'RB RMB MRB MRMB no drop point': [2, [0, 0]],
                'LF LMF MLF MLMF no drop point': [2, [0, 0]],
                'LB LMB MLB MLMB no drop point': [2, [0, 0]],
                'Unexpected ball': null,
            },
            template: valParam => {
                const [realOW, realW, realIW, realH] = [12.8, 10.97, 8.23, 11.885];
                const u = bs(1) / realOW;
                const w = u * realW, h = u * realH;
                const iw = u * realIW;
                const style = {fill: 'none', strokeWidth: 1}
                return <g>
                    {/* 球场 */}
                    <g>
                        {/* 背景 */}
                        <rect x={-w / 2} y={-h / 2} width={w} height={h}
                              {...style}/>
                        {/* 球网 */}
                        <line x1={-u * realOW / 2} y1={h / 2}
                              x2={u * realOW / 2} y2={h / 2}
                              {...style}/>
                        {/* 单双打分界线 */}
                        {[-1, 1].map(dir => {
                            const x = dir * iw / 2;
                            return <line key={dir}
                                         x1={x} y1={-h / 2}
                                         x2={x} y2={h / 2}
                                         {...style}/>
                        })}
                        {/* 前后场分界线 */}
                        <line x1={-iw / 2} y1={0} x2={iw / 2} y2={0}
                              {...style}/>
                        {/* 左右场分界线 */}
                        <line x1={0} y1={0} x2={0} y2={h / 2}
                              {...style}/>
                    </g>
                    {valParam && <g transform={`translate(${-iw / 2}, ${-h / 2})`}>
                        {/* serving */}
                        {valParam[0] === 0 && (function () {
                            return <g transform={`translate(${valParam[1] * iw / 6}, ${h / 2})`}>
                                <rect x={0} y={0} width={iw / 6} height={h / 2}
                                      strokeWidth={0}/>
                                <path stroke={'white'}
                                      strokeWidth={1}
                                      fill={'none'}
                                      d={`M${iw / 6 - 1} ${h / 8}L${iw / 12} 1 L1 ${h / 8}L${iw / 6 - 1} ${h * 3 / 8}L${iw / 12} ${h / 2 - 1}L1 ${h * 3 / 8}`}/>
                            </g>
                        })()}
                        {/* normal */}
                        {valParam[0] === 1 && (function () {
                            const [x, y] = valParam[1];
                            return <rect transform={`translate(${x * iw / 4}, ${y * h / 4})`}
                                         x={0} y={0} width={iw / 4} height={h / 4}
                                         strokeWidth={0}/>
                        })()}
                        {/* no drop */}
                        {valParam[0] === 2 && (function () {
                            const [x, y] = valParam[1];
                            return <g transform={`translate(${x * iw / 2}, ${y * h / 2})`}>
                                <rect x={0} y={0} width={iw / 2} height={h / 2}
                                      strokeWidth={0}/>
                                <line x1={0} y1={0}
                                      x2={iw / 2} y2={h / 2}
                                      strokeWidth={1}
                                      stroke={'white'}
                                      fill={'none'}/>
                                <line x1={iw / 2} y1={0}
                                      x2={0} y2={h / 2}
                                      strokeWidth={1}
                                      stroke={'white'}
                                      fill={'none'}/>
                            </g>
                        })()}
                    </g>}
                </g>;
            }
        },
        'Hitting Pose': {
            pos: (size, asGlyph) => asGlyph ?
                `scale(${size[0] / bs(1)},${size[1] / bs(1)})` :
                `scale(${size[0] / bs(1)},${size[1] / bs(1)})`,
            params: {
                'Starting Tee 1': [0, 1],
                'Starting Tee 2': [0, 0],
                'Starting Tee 3': [0, 2],
                'Starting Tee 4': [0, 3],
                'Back sideway': [1, 0],
                'Forehand': [1, 1],
                'Backhand': [1, 2],
                'Sideway': [1, 3],
                'Untouch': null,
            },
            template: valParam => {
                const or = bs(.5),
                    strokeWidth = 1,
                    ior = or - strokeWidth,
                    ir = bs(.44);
                return <g>
                    <path d={`M${-or} 0A${or} ${or} 0 0 1 ${or} 0H${ior}A${ior} ${ior} 0 0 0 ${-ior} 0Z`}
                          strokeWidth={0}/>
                    {[...new Array(4)].map((_, i) => {
                        const pr = 0.1;
                        const dr = Math.PI / 4 * (1 - pr);
                        const r2 = Math.PI / 4 * i + pr / 2 * Math.PI / 4, r1 = r2 + dr;
                        const [x1, y1] = r2xy(r1, or),
                            [x2, y2] = r2xy(r2, or),
                            [x3, y3] = r2xy(r2, ir),
                            [x4, y4] = r2xy(r1, ir);
                        const isFilled = valParam && valParam[0] === 1 && valParam[1] === i;
                        return <path key={i}
                                     strokeWidth={1}
                                     d={`M${x1} ${y1}A${or} ${or} 0 0 0 ${x2} ${y2}L${x3} ${y3}A${ir} ${ir} 0 0 1 ${x4} ${y4}Z`}
                                     fill={isFilled ? undefined : '#fff'}/>
                    })}
                    {valParam && valParam[0] === 0 && (function () {
                        const [cx, cy] = r2xy(Math.PI / 4 * valParam[1] + Math.PI / 8, (ir + or) / 2)
                        return <circle cx={cx}
                                       cy={cy}
                                       r={(or - ir) / 2}
                                       strokeWidth={0}/>
                    })()}
                </g>;
            }
        },
        'Hit Technique': {
            pos: (size, asGlyph) => asGlyph ?
                `translate(0, ${0.27 * size[1]}) scale(${size[0] * 0.6 / bs(1)},${size[1] * 0.6 / bs(1)})` :
                `scale(${size[0] / bs(1)},${size[1] / bs(1)})`,
            params: {
                'Overhand serving': 'O.S.',
                'Underhand serving': 'U.S.',
                'Drive': 'Dri',
                'Drive in the air': 'Air',
                'Smash': 'Sma',
                'Ground smash': 'Grd',
                'Volley': 'Vol',
                'Drop shot': 'Drop',
                'Push and block': 'Push',
                'Half volley': "HVol",
                'Lob': 'Lob',
                'Slice': 'Sli',
                'Other': 'Oth',
            },
            template: valParam => {
                return <text textAnchor={'middle'}
                             dominantBaseline={'middle'}
                             fontSize={bs(0.4)}
                             strokeWidth={0}>
                    {valParam}
                </text>;
            }
        },
    },
    'Table Tennis': {
        'Hitting Tech': {},
        'Player Pos': {},
        'Ball Pos': {},
        'Spin': {},
    },
}

class Renderer {
    pos = () => '';
    _size = [100, 100];
    template = () => null;
    params = {};
    _color = 'black';

    constructor(glyphConfig) {
        if (!glyphConfig) return;
        this.pos = glyphConfig.pos || this.pos;
        this.template = glyphConfig.template || this.template;
        this.params = glyphConfig.params || this.params;
    }

    size(size) {
        this._size = [size, size];
        return this;
    }

    color(color) {
        this._color = color;
        return this;
    }

    render(value, asGlyph=true) {
        return <g transform={this.pos(this._size, asGlyph)}>
            {this.template(this.params[value]) || null}
        </g>
    }
}

class SymbolSet {
    constructor(ds) {
        this.dsName = ds;
        this.glyphSet = null;
        this.glyphSetName = null;
        for (const glyphSetName of Object.keys(symbolSets))
            if (ds.startsWith(glyphSetName)) {
                this.glyphSetName = glyphSetName;
                this.glyphSet = symbolSets[glyphSetName]
            }
        if (!this.glyphSet)
            console.warn('Unrecognized dataset name: ' + ds);
    }

    attr(attr) {
        if (!this.glyphSet) return new Renderer(null);
        if (this.glyphSet.hasOwnProperty(attr)) return new Renderer(this.glyphSet[attr]);
        console.warn(`Attribute ${attr} is not in the glyph set of ${this.glyphSetName}!`)
    }

    static dataset(ds) {
        return new SymbolSet(ds);
    }
}

export default SymbolSet;
