#include "sIF_lfs.h"
#include "sBSP_F1_DWT.h"


extern lfs_t lfs_w25qxx;

//#define USE_TIME_MEASURE

/*
 * @brief littlefs read interface
 * @param [in] c lfs_config数据结构
 * @param [in] block 要读的块
 * @param [in] off 在当前块的偏移
 * @param [out] buffer 读取到的数据
 * @param [in] size 要读取的字节数
 * @return 0 成功 <0 错误
 * @note littlefs 一定不会存在跨越块存储的情况
 */
int sMid_IFS_PortRead(const struct lfs_config *c, lfs_block_t block,
                      lfs_off_t off, void *buffer, lfs_size_t size){
    #ifdef USE_TIME_MEASURE
    sBSP_DWT_MeasureStart();
    #endif
    sDrv_W25Qxx_ReadBytes(buffer,block * (W25Qxx_Info.erase_gran) + off,size);
    #ifdef USE_TIME_MEASURE
    sBSP_DWT_MeasureEnd();
    sHMI_Debug_Printf("_READ T: %d uS \n",sBSP_DWT_GetMeasure_us());
    #endif
    return LFS_ERR_OK;
}
       

/*
 * @brief littlefs write interface
 * @param [in] c lfs_config数据结构
 * @param [in] block 要读的块
 * @param [in] off 在当前块的偏移
 * @param [out] buffer 读取到的数据
 * @param [in] size 要读取的字节数
 * @return 0 成功 <0 错误
 * @note littlefs 一定不会存在跨越块存储的情况
 */
int sMid_IFS_PortWrite(const struct lfs_config *c, lfs_block_t block,
				       lfs_off_t off,const void *buffer, lfs_size_t size){
    #ifdef USE_TIME_MEASURE
    sBSP_DWT_MeasureStart();
    #endif
    sDrv_W25Qxx_WriteBytes((uint8_t*)buffer,block * (W25Qxx_Info.erase_gran) + off,size);
    #ifdef USE_TIME_MEASURE
    sBSP_DWT_MeasureEnd();
    sHMI_Debug_Printf("_WRITE T: %d uS\n",sBSP_DWT_GetMeasure_us());
    #endif
    
    return LFS_ERR_OK;
}
                       
/*
 * @brief littlefs 擦除一个块
 * @param [in] c lfs_config数据结构
 * @param [in] block 要擦出的块
 * @return 0 成功 <0 错误
 */
int sMid_IFS_PortErase(const struct lfs_config *c, lfs_block_t block){
    #ifdef USE_TIME_MEASURE
    sBSP_DWT_MeasureStart();
    #endif
    sDrv_W25Qxx_SectorErase(block);
    #ifdef USE_TIME_MEASURE
    sBSP_DWT_MeasureEnd();
    sHMI_Debug_Printf("_ERASE T: %d uS\n",sBSP_DWT_GetMeasure_us());
    #endif
    return LFS_ERR_OK;
}


int sMid_IFS_PortSync(const struct lfs_config *c){
	return  LFS_ERR_OK;
}


lfs_t lfs_w25qxx;

struct lfs_config w25qxx_cfg = {
    .read  = sMid_IFS_PortRead,
    .prog  = sMid_IFS_PortWrite,
    .erase = sMid_IFS_PortErase,
    .sync  = sMid_IFS_PortSync,
    
    .read_size = 256,
    .prog_size = 256,
    //.block_size = 4096,
    //.block_count = 4096,
    .cache_size = 512,
    .lookahead_size = 512,
    .block_cycles = 500,
    };

int8_t sMid_IFS_Init(){
    
    
    if(sDrv_W25Qxx_Init() != 0){
        return -1;
    }
    
    w25qxx_cfg.block_size = W25Qxx_Info.erase_gran;
    w25qxx_cfg.block_count = W25Qxx_Info.sector_num;
    
    int8_t err_code = -1;
    
    err_code = (int8_t)lfs_mount(&lfs_w25qxx,&w25qxx_cfg);
    if(err_code){
        lfs_format(&lfs_w25qxx, &w25qxx_cfg);
		err_code = lfs_mount(&lfs_w25qxx, &w25qxx_cfg);
        sHMI_Debug_Printf("格式化, 错误代码: %d\n",err_code);
    }
    
    return err_code;
}




