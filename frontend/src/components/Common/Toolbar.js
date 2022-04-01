import {Box, Tooltip} from "@mui/material";

export default function Toolbar({elements}) {
    return <Box display={'flex'}
                height={'100%'}
                justifyContent={'right'}
                alignItems={'center'}>
        {elements.map((ele, eId) => (
            ele.title ?
                <Tooltip key={eId}
                         title={ele.title}>
                    <Box display={'flex'}
                         flex={'0 0 auto'}
                         height={'100%'}
                         alignItems={'center'}
                         mr={1}>
                        {ele.content}
                    </Box>
                </Tooltip> :
                <Box key={eId}
                     display={'flex'}
                     flex={'0 0 auto'}
                     height={'100%'}
                     alignItems={'center'}
                     mr={1}>
                    {ele}
                </Box>
        ))}
    </Box>
}
