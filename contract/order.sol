// SPDX-License-Identifier: MIT
pragma solidity 0.8.10;
import {IUniswapV2Router02} from "@uniswap/v2-periphery/contracts/interfaces/IUniswapV2Router02.sol";
import {ISwapRouter} from "@uniswap/v3-periphery/contracts/interfaces/ISwapRouter.sol";
import {TransferHelper} from "@uniswap/v3-periphery/contracts/libraries/TransferHelper.sol";
import {IUniswapV3Pool} from "@uniswap/v3-core/contracts/interfaces/IUniswapV3Pool.sol";
import {IERC20} from "https://github.com/OpenZeppelin/openzeppelin-contracts/blob/release-v4.9/contracts/token/ERC20/IERC20.sol";
import {SafeERC20} from "https://github.com/OpenZeppelin/openzeppelin-contracts/blob/release-v4.9/contracts/token/ERC20/utils/SafeERC20.sol";
import {SafeMath} from "https://github.com/OpenZeppelin/openzeppelin-contracts/blob/release-v4.9/contracts/utils/math/SafeMath.sol";

import "hardhat/console.sol";

interface customeInterface {
    function token0() external view returns (address);
    function token1() external view returns (address);
    function getReserves()external view returns (uint112 reserve0,uint112 reserve1,uint32 blockTimestampLast);
    function getPair(address tokenA, address tokenB)external view returns (address pair);
    function getAmountsOut(uint amountIn, address[] calldata path) external view returns (uint[] memory amounts);
    function getAmountsIn(uint256 amountOut, address[] calldata path) external view returns (uint256[] memory amounts);
}

contract Order {
    using SafeMath for uint256;
    address public owner;

    constructor() {
        owner = msg.sender;
    }

    modifier onlyOwner() {
        require(msg.sender == owner, "I don't work for stranger");
        _;
    }
    
    struct FlashCallbackData {
        uint256 amount0;//token0 amount to borrow from the pool
        uint256 amount1; //token1 amount to borrow from the pool
        //address caller; //this contracts address
        address[2] path; //trade path
        //uint8[3] exchRoute; //route pattern
        address[2] dexs;//router addresses to use
        uint256[2] ExpAmt;//expected aoutput amounts through the path
        //uint24 fee;//fee if i use v3 forks
        address[3] loanInfo;
    }

    function FlashloanRequest(
        address[2] memory _path,
        address[2] memory _dexs,
        uint256 _amount0,
        uint256 _amount1,
        uint256[2] memory _minOut,
        address[3] memory _loanInfo
    ) public onlyOwner {
        bytes memory data = abi.encode(
            FlashCallbackData({
                amount0: _amount0,
                amount1: _amount1,
                path: _path,
                dexs: _dexs,
                ExpAmt: _minOut,
                loanInfo: _loanInfo
            })
        );
        
        IUniswapV3Pool pool; 
        pool= IUniswapV3Pool(_loanInfo[0]);
        IUniswapV3Pool(pool).flash(address(this), _amount0, _amount1, data);
        console.log("here flashloan finished ");
    }

    function pancakeV3FlashCallback(uint256 fee0,uint256 fee1,bytes calldata data) external
     {
        FlashCallbackData memory decoded = abi.decode(data,(FlashCallbackData));
        require(msg.sender == address(decoded.loanInfo[0]),"not authorized pool address to loan");
        IERC20 token0;
        IERC20 token1;
        token0 = IERC20(decoded.loanInfo[1]);
        token1 = IERC20(decoded.loanInfo[2]);
        //initially
        IERC20 baseToken = (fee0 > 0) ? token0 : token1;
        uint256 acquiredAmount = (fee0 > 0) ? decoded.amount0 : decoded.amount1;
        console.log("expected from t1: ",decoded.ExpAmt[0]);
        /*trade1*/
        acquiredAmount = PlaceSwap(
            decoded.dexs[0],
            acquiredAmount,
            [address(baseToken), decoded.path[0]],
           // 0,//route pattern
            decoded.ExpAmt[0]
           // 0//fee
        );
        console.log("expected from t2: ",decoded.ExpAmt[1]);
        /*trade2*/
        acquiredAmount = PlaceSwap(
            decoded.dexs[1],
            acquiredAmount,
            [decoded.path[0], decoded.path[1]],
            //0,//route pattern
            decoded.ExpAmt[1]
            //0//fee
        );

        uint256 FinalExpextedAmount=acquiredAmount.mul(990).div(1000);//only 0.9% slipage tolerance when try to convert to valubale token for loanrepay
        console.log("expected from t3: ",FinalExpextedAmount);
        if(decoded.path[1]!=address(baseToken))
        {
            /*trade3 aka repay*/
        acquiredAmount = PlaceSwap(
            decoded.dexs[1], //must fix!!!!!!!!
            acquiredAmount,
            [decoded.path[1], address(baseToken)],
           // 0,//route pattern
            FinalExpextedAmount
           // 0//fee
        );
        }

        /*loan repayment and profit collection process*/
        if(acquiredAmount<decoded.amount1+fee1)
        {
            console.log("gained: ",acquiredAmount);
            console.log("payable: ",decoded.amount1+fee1);
            revert("can not pay loan");
        }else if (fee0 > 0) {
            TransferHelper.safeApprove(address(token0),address(this),token0.balanceOf(address(this)));
            token0.transfer( address(decoded.loanInfo[0]),decoded.amount0 + fee0 ); //fix here !!!!!!!!!!!!!!
            token0.transfer(address(owner),token0.balanceOf(address(this)));//danger!!!!!!!!
        } else {
            TransferHelper.safeApprove(address(token1),address(this),token1.balanceOf(address(this)));
            token1.transfer(address(decoded.loanInfo[0]),decoded.amount1 + fee0); //fix here !!!!!!!!!!!!!!
            token1.transfer(address(owner),token1.balanceOf(address(this)));//danger!!!!!!!!
        }
    }

    function PlaceSwap(address dex,uint256 _amountIn,address[2] memory _tokenPath,uint256 _minOut) private returns (uint256) {
        uint256 deadline = block.timestamp + 10000;
        uint256 swap_amount_out = 0;//assuming swap amount result is zero
        address[] memory path = new address[](2);
        path[0] = _tokenPath[0];
        path[1] = _tokenPath[1];


            IUniswapV2Router02 pancakeswapV2 = IUniswapV2Router02(dex); // assigning dex
            TransferHelper.safeApprove(_tokenPath[0],address(pancakeswapV2),_amountIn);
            swap_amount_out = pancakeswapV2.swapExactTokensForTokens({
                amountIn: _amountIn,
                amountOutMin: _minOut,
                path: path,
                to: address(this),
                deadline: deadline
            })[1];
            console.log("\t\t swap passed");
        
       
        return swap_amount_out;
    }
     receive() external payable {}

    function withdrawToken(address token, uint256 amount) external onlyOwner {
        require(token != address(0), "Invalid token address");
        require(amount > 0, "Amount must be greater than 0");

        (bool success, ) = token.call(
            abi.encodeWithSignature("transfer(address,uint256)", owner, amount)
        );
        require(success, "Transfer failed");
    }

    // Allow the owner to withdraw Ether from the contract
    function withdrawEther(uint256 amount) external onlyOwner {
        require(amount > 0, "Amount must be greater than 0");
        (bool success, ) = payable(owner).call{value: amount}("");
        require(success, "Withdrawal failed");
    }
}
