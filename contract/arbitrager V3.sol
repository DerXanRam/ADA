// SPDX-License-Identifier: MIT
pragma solidity <=0.8.10 >=0.8.0;
import {SafeMath} from "https://github.com/OpenZeppelin/openzeppelin-contracts/blob/release-v4.9/contracts/utils/math/SafeMath.sol";

interface customeInterface {
    function token0() external view returns (address);
    function token1() external view returns (address);
    function getReserves()external view returns (uint112 reserve0,uint112 reserve1,uint32 blockTimestampLast);
    function getPair(address tokenA, address tokenB)external view returns (address pair);
    function getAmountsOut(uint amountIn, address[] calldata path) external view returns (uint[] memory amounts);
    function getAmountsIn(uint256 amountOut, address[] calldata path) external view returns (uint256[] memory amounts);
}

contract TargetSpotted {
    using SafeMath for uint256;
   
   function makePeggedPair(address tokenx, address factory,address valubale) internal view returns (bool result, address pairPool,address[] memory pair, address[3] memory loanInfo)//--optimization s needed
    {
        result = false;
        pair = new address[](2);
        address[4] memory peggeds = [
            0x55d398326f99059fF775485246999027B3197955,//usdt
            0xe9e7CEA3DedcA5984780Bafc599bD69ADd087D56,//busd
            0x8AC76a51cc950d9822D68b83fE1Ad97B32Cd580d,//usdc
            0xbb4CdB9CBd36B01bD1cBaEBF2De08d9173bc095c //wbnb
        ];
        for (uint256 i = 0; i < 4; i++) 
        {
            if(peggeds[i]!=tokenx && peggeds[i]!=valubale)
            {
                pairPool = customeInterface(factory).getPair(tokenx,peggeds[i]);
                if (pairPool != address(0)) 
                {
                result = true;
                pair[0] = peggeds[i];
                pair[1] = tokenx;
                if(i==0)
                {
                    loanInfo[0]=0xd881D9d0e0767719701305c614903F555d589586;
                    loanInfo[1]=0x40af3827F39D0EAcBF4A168f8D4ee67c121D11c9;
                    loanInfo[2]=0x55d398326f99059fF775485246999027B3197955;
                }else if(i==1){
                    loanInfo[0]=0x85FAac652b707FDf6907EF726751087F9E0b6687;
                    loanInfo[1]=0xbb4CdB9CBd36B01bD1cBaEBF2De08d9173bc095c;
                    loanInfo[2]=0xe9e7CEA3DedcA5984780Bafc599bD69ADd087D56;
                }
                else if(i==2){
                    loanInfo[0]=0x92b7807bF19b7DDdf89b706143896d05228f3121;
                    loanInfo[1]=0x55d398326f99059fF775485246999027B3197955;
                    loanInfo[2]=0x8AC76a51cc950d9822D68b83fE1Ad97B32Cd580d;

                }else if(i==3){
                    loanInfo[0]=0x36696169C63e42cd08ce11f5deeBbCeBae652050;
                    loanInfo[1]=0x55d398326f99059fF775485246999027B3197955;
                    loanInfo[2]=0xbb4CdB9CBd36B01bD1cBaEBF2De08d9173bc095c;
                }
                break;
              }//must finish the else to decrease path if it is vaulable token
            }
        }
    }
    
    struct varflags
    {
        bool flag_pool;
        bool flag_maths;
        bool res;
        address token0;
        uint reserve0;
        uint reserve1;
        uint denominator;
        uint256 res1;
        uint256 res2;
    }

    function valuableToken(address tokenToCheck) internal pure returns (bool val) {
        val=false;
         address[3] memory peggeds = [
            0x55d398326f99059fF775485246999027B3197955,//usdt
            0xe9e7CEA3DedcA5984780Bafc599bD69ADd087D56,//busd
            0x8AC76a51cc950d9822D68b83fE1Ad97B32Cd580d//usdc
        ];
        for(uint256 i=0;i<3;i++)
        {
            if(tokenToCheck==peggeds[i])
            {
                val=true;
                break;
            }
        }
        return val;
    } 
   
    function sortTokens(address tokenA, address tokenB) internal pure returns (address token0, address token1) {
        require(tokenA != tokenB, 'sortToken(): IDENTICAL_ADDRESSES');
        (token0, token1) = tokenA < tokenB ? (tokenA, tokenB) : (tokenB, tokenA);
        require(token0 != address(0), 'sortToken(): ZERO_ADDRESS');
    }
    
    struct flag_done{
        bool done;
         uint256[] MinInputs;//var to store collected prices from different dex for searching arbitrage opportunity
        address[3] dexs;//used dexs to search low price
        address[2] dexPath;//dex rougths to be used for arbitarge
        address[2] tradePath;//path to be used for arbitrage 
    }

    function searchInFromDexs(address outToken, uint256 outAmt,address to,address vauable) internal view returns(uint256[] memory minIns, address[5] memory routers,address[5][3] memory loanInfo)//size of loanInfo must fix
    {
        varflags memory vars;
        routers = 
        [0x10ED43C718714eb63d5aA57B78B54704E256024E,//pancake v2 * 
        0x3a6d8cA21D1CF76F653A67577FA0D27453350dD8,//biswap v2 router
        0x1b02dA8Cb0d097eB8D57A175b88c7D8b47997506,//sushiswap v2 router
        0xcF0feBd3f17CEf5b47b0cD257aCf6025c5BFf3b7,//apeswap
        0x7DAe51BD3E3376B8c7c4900E9107f12Be3AF1bA8];//medex
        address[5] memory factory = 
        [0xcA143Ce32Fe78f1f7019d7d551a6402fC5350c73,//pancake v2 factory  
        0x858E3312ed3A876947EA49d572A7C42DE08af7EE,//biswapPair
        0xc35DADB65012eC5796536bD9864eD8773aBc74C4,//sushiV2factory
        0x0841BD0B734E4F5853f0dD8d7Ea041c241fb0Da6,//apeswap
        0x3CD1C46068dAEa5Ebb0d3f55F6915B10648062B8];//medex

        vars.flag_pool=false;
        vars.flag_maths=false;
        minIns = new uint256[](routers.length);
        uint256 amCounter = 0;
        for (uint256 i = 0; i < routers.length; i++) 
        {
            address[] memory pair;
             address pool;
             address[3] memory loan;
             if(to==routers[i])(vars.res,pool,pair,loan) = makePeggedPair(outToken,factory[i],vauable);
             else (vars.res,pool,pair,loan) =makePeggedPair(outToken,factory[i],address(0));
             loanInfo[0][i]=loan[0];
             loanInfo[1][i]=loan[1];
             loanInfo[2][i]=loan[2];
             if(vars.res)
             {
                /*getting pool and reserve*/
               (vars.token0,) = sortTokens(pair[0], pair[1]);
               (vars.reserve0, vars.reserve1,) = customeInterface(pool).getReserves();
               
               uint256 res2;
                (res2) = pair[0] == vars.token0 ? (vars.reserve1) : (vars.reserve0);
               /*getting reserve finished*/
              
               vars.flag_pool=true;
               if (res2>outAmt) 
               {
                   minIns[amCounter] = customeInterface(routers[i]).getAmountsIn(outAmt, pair)[0];
                   amCounter++;
                   vars.flag_maths=true;
                } else {
                   minIns[amCounter] = 0;
                   amCounter++;
                }
             }
        }

        if(vars.flag_pool && !vars.flag_maths)
        {
             revert("pool found but mathERR");
             
        }else if(!vars.flag_pool && !vars.flag_pool)
        {
            revert("can't make pegged");
        }

    }
    function searchOutFromDexs(address inToken, uint256 inAmt,address to,address vauable) internal view returns(uint256[] memory maxOuts, address[5] memory routers,address[5][3] memory loanInfo)//size of loanInfo must fix
    {
        varflags memory vars;
       routers = 
        [0x10ED43C718714eb63d5aA57B78B54704E256024E,//pancake v2 * 
        0x3a6d8cA21D1CF76F653A67577FA0D27453350dD8,//biswap v2 router
        0x1b02dA8Cb0d097eB8D57A175b88c7D8b47997506,//sushiswap v2 router
        0xcF0feBd3f17CEf5b47b0cD257aCf6025c5BFf3b7,//apeswap
        0x7DAe51BD3E3376B8c7c4900E9107f12Be3AF1bA8];//medex
        address[5] memory factory = 
        [0xcA143Ce32Fe78f1f7019d7d551a6402fC5350c73,//pancake v2 factory  
        0x858E3312ed3A876947EA49d572A7C42DE08af7EE,//biswapPair
        0xc35DADB65012eC5796536bD9864eD8773aBc74C4,//sushiV2factory
        0x0841BD0B734E4F5853f0dD8d7Ea041c241fb0Da6,//apeswap
        0x3CD1C46068dAEa5Ebb0d3f55F6915B10648062B8];//medex

        vars.flag_pool=false;
        vars.flag_maths=false;
        maxOuts = new uint256[](routers.length);
        uint256 amCounter = 0;
        for (uint256 i = 0; i <routers.length; i++) 
        {
             address[] memory pair;
             address pool;
             address[3] memory loan;
             if(to==routers[i]) (vars.res,pool,pair,loan) = makePeggedPair(inToken,factory[i],vauable);
             else (vars.res,pool,pair,loan) =makePeggedPair(inToken,factory[i],address(0));
             loanInfo[0][i]=loan[0];
             loanInfo[1][i]=loan[1];
             loanInfo[2][i]=loan[2];
             if(vars.res)
             {
                /*getting pool and reserve*/
               (vars.token0,) = sortTokens(pair[0], pair[1]);
               (vars.reserve0, vars.reserve1,) = customeInterface(pool).getReserves();
               
                (vars.res1) = pair[0] == vars.token0 ? (vars.reserve0) : (vars.reserve1);
                (vars.res2) = pair[0] == vars.token0 ? (vars.reserve1) : (vars.reserve0);
               // vars.denominator = inAmt.mul(998).mul(vars.res2)/vars.res1.mul(1000).add(inAmt.mul(998));//fix here
               /*getting reserve finished*/

               vars.flag_pool=true;
               if (true) //fix hereeeeererere s  e 
               {
                address[] memory tokenPath=new address[](2);
                tokenPath[0]=pair[1];
                tokenPath[1]=pair[0];
                maxOuts[amCounter] = customeInterface(routers[i]).getAmountsOut(inAmt, tokenPath)[1];
                amCounter++;
                vars.flag_maths=true;
                } else {
                maxOuts[amCounter] = 0;
                amCounter++;
                }
             }
        }
        
        if(vars.flag_pool && !vars.flag_maths)
        {
             revert("pool found but mathERR");
        }else if(!vars.flag_pool && !vars.flag_pool)
        {
            revert("can't make pegged");
        }
    }
    function getAmountOut(uint amountIn, uint reserveIn, uint reserveOut) internal pure returns (uint amountOut) {
        require(amountIn > 0, 'getAmountout(): INSUFFICIENT_INPUT_AMOUNT');
        require(reserveIn > 0 && reserveOut > 0, 'getAmountout(): INSUFFICIENT_LIQUIDITY');
        uint amountInWithFee = amountIn.mul(998);
        uint numerator = amountInWithFee.mul(reserveOut);
        uint denominator = reserveIn.mul(1000).add(amountInWithFee);
        amountOut = numerator / denominator;
    }
     function getAmountIn(uint amountOut, uint reserveIn, uint reserveOut) internal pure returns (uint amountIn) {
        require(amountOut > 0, 'PancakeLibrary: INSUFFICIENT_OUTPUT_AMOUNT');
        require(reserveIn > 0 && reserveOut > 0, 'PancakeLibrary: INSUFFICIENT_LIQUIDITY');
        uint numerator = reserveIn.mul(amountOut).mul(1000);
        uint denominator = reserveOut.sub(amountOut).mul(998);
        amountIn = (numerator / denominator).add(1);
    }

    function getAmountsOut(address factory,uint256 inAmt,address[] memory tokenPath,uint256 usage) internal view returns(uint256 impactedMaxOut, uint256 nonimpactedMaxOut)
    {
        /*getting pool and reserve*/
        address pool=customeInterface(factory).getPair(tokenPath[0], tokenPath[1]);
        if(pool==address(0)) revert("getAmountsOut(); no pool for sent tnx");
        (address token0,) = sortTokens(tokenPath[0], tokenPath[1]);
        (uint256 reserve0, uint256 reserve1,) = customeInterface(pool).getReserves();
        uint256 res1;
        uint256 res2;
        (res1) = tokenPath[0] == token0 ? (reserve0) : (reserve1);
        (res2) = tokenPath[0] == token0 ? (reserve1) : (reserve0);
        uint256 minIn=nonimpactedMaxOut=getAmountOut(inAmt,res1,res2);
        res1+=inAmt;
        res2-=minIn;
        //nonimpactedMaxOut=minIn=minIn.mul(100).div(1000);
        impactedMaxOut=getAmountOut(minIn.mul(usage).div(1000),res2,res1);
        nonimpactedMaxOut=nonimpactedMaxOut.mul(usage).div(1000);  
    }
    function getAmountsIn(address factory,uint256 outAmt,address[] memory tokenPath,uint256 usage) internal view returns(uint256 nonimpactedMinIn,uint256 impactedMinIn)
    {      
        /*getting pool and reserve*/
        address pool=customeInterface(factory).getPair(tokenPath[0], tokenPath[1]);
        (address token0,) = sortTokens(tokenPath[0], tokenPath[1]);
        if(pool==address(0)) revert("getAmountsIn(); no pool for sent tnx");
        (uint256 reserve0, uint256 reserve1,) = customeInterface(pool).getReserves();
        uint256 res1;
        uint256 res2;
        (res1) = tokenPath[0] == token0 ? (reserve0) : (reserve1);
        (res2) = tokenPath[0] == token0 ? (reserve1) : (reserve0);
        uint256 minIn=nonimpactedMinIn=getAmountOut(outAmt,res1,res2);
        nonimpactedMinIn=nonimpactedMinIn.mul(usage).div(1000);
        res1+=outAmt;
        res2-=minIn;
        impactedMinIn=getAmountOut(minIn.mul(usage).div(1000),res2,res1);
    }

    struct todeep{
        uint256 maxOut;
        uint256[] minIns;
        address[5] routers;
        address[5][3] loanInfo;

        uint256 minIn;
        uint256[] maxOuts;
        uint256 HeGet;
        uint256 Iget;
        uint256 minThreshold;
    }

    function getBnBconversion(uint256 BNBAmount,address pegged)internal view returns (uint256 amountInPegged)
    {
        address[] memory temp=new address[](2);
        temp[0]=0xbb4CdB9CBd36B01bD1cBaEBF2De08d9173bc095c;
        temp[1]=pegged;
        amountInPegged=customeInterface(0x10ED43C718714eb63d5aA57B78B54704E256024E).getAmountsOut(BNBAmount,temp)[1];
    }
    function AnalyseTnx(address to,address factory,address[] memory tokenPath,uint256 amtIn,uint256 usage)external view returns (uint256[2] memory expecteds,uint256 startBalance, address[4] memory totalPath,address[3] memory loancopyed)
    {

        if(valuableToken(tokenPath[0]))
        {
            todeep memory vars;
            (vars.Iget,vars.maxOut)=getAmountsOut(factory,amtIn,tokenPath,usage);//must fix with ipacted reserve calculation
            (vars.minIns,vars.routers,vars.loanInfo)=searchInFromDexs(tokenPath[1],vars.maxOut,to,tokenPath[0]);
            for(uint256 i=0;i<vars.routers.length;i++)
            {
                loancopyed[0]=vars.loanInfo[0][i];
                loancopyed[1]=vars.loanInfo[1][i];
                loancopyed[2]=vars.loanInfo[2][i];
                if(loancopyed[2]==0xbb4CdB9CBd36B01bD1cBaEBF2De08d9173bc095c) vars.minThreshold=getBnBconversion(vars.minIns[i],tokenPath[0]);
                else vars.minThreshold=vars.minIns[i];
                if(vars.minThreshold<vars.Iget && vars.minThreshold!=0 && (vars.Iget-vars.minThreshold)>=55e16) 
                {
                    startBalance=vars.minIns[i];
                    totalPath=[tokenPath[1],tokenPath[0],vars.routers[i],to];
                    expecteds=[uint256(vars.maxOut),uint256(vars.Iget+89e16)];
                    break;
                }else
                {
                    revert("analyse()->val: not profitable");
                }
            }
        }else if(valuableToken(tokenPath[1]))
        {
            todeep memory vars;
            address[] memory tempPath=new address[](2);
            tempPath[0]=tokenPath[1];
            tempPath[1]=tokenPath[0];
            (vars.HeGet,vars.minIn)=getAmountsIn(factory,amtIn,tokenPath,usage);//must fix with ipacted reserve calculation

            (vars.maxOuts,vars.routers,vars.loanInfo)=searchOutFromDexs(tokenPath[0],vars.minIn,to,tokenPath[1]);
            
            for(uint256 i=0;i<vars.routers.length;i++)
            {
                loancopyed[0]=vars.loanInfo[0][i];
                loancopyed[1]=vars.loanInfo[1][i];
                loancopyed[2]=vars.loanInfo[2][i];
                if(loancopyed[2]==0xbb4CdB9CBd36B01bD1cBaEBF2De08d9173bc095c) vars.minThreshold=getBnBconversion(vars.maxOuts[i],tokenPath[1]);
                else vars.minThreshold=vars.maxOuts[i];
                if(vars.minThreshold>vars.HeGet && vars.minThreshold!=0 && (vars.minThreshold-vars.HeGet)>55e16)
                {
                    startBalance=vars.HeGet;
                    totalPath=[tokenPath[0],tokenPath[1],to,vars.routers[i]];
                    expecteds=[uint256(vars.minIn),uint256(vars.maxOuts[i].mul(990).div(1000))];
                    break;
                }else
                {
                    revert("analyse()->non-val: not profitable");
                }
            }
        }else{
            revert("analyze(): both tokens are not valubale");
        }
    }
    
}
