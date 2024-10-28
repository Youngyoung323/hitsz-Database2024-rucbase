/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "rm_scan.h"
#include "rm_file_handle.h"

/**
 * @brief 初始化file_handle和rid
 * @param file_handle
 */
RmScan::RmScan(const RmFileHandle *file_handle) : file_handle_(file_handle) {
    // Todo:
    // 初始化file_handle和rid（指向第一个存放了记录的位置）

    // 初始化file_handle
    this->file_handle_ = file_handle;
    // 初始化rid
    rid_.page_no = RM_FIRST_RECORD_PAGE;
    rid_.slot_no = -1;  // 索引从0开始
    // 调用next函数，找到第一个存放了记录的非空闲位置
    next();
}

/**
 * @brief 找到文件中下一个存放了记录的位置
 */
void RmScan::next() {
    // Todo:
    // 找到文件中下一个存放了记录的非空闲位置，用rid_来指向这个位置
    if(is_end()) {
        return;
    }
    else {
        while(rid_.page_no < file_handle_->file_hdr_.num_pages) {
            // 找到当前page中第一个存放了记录的非空闲位置
            rid_.slot_no = Bitmap::next_bit(true, file_handle_->fetch_page_handle(rid_.page_no).bitmap, 
                                            file_handle_->file_hdr_.num_records_per_page, rid_.slot_no);
            if(this->rid_.slot_no >= this->file_handle_->file_hdr_.num_records_per_page) {  
                // 本页面没有找到存放记录的非空闲位置
                if((rid_.page_no + 1) == this->file_handle_->file_hdr_.num_pages) {  // num_pages初始化为1
                    // 超过了最大页面数量
                    this->rid_ = {RM_NO_PAGE, -1};
                    return;
                }
                else {
                    this->rid_ = {this->rid_.page_no + 1, -1};
                }
            }
            else {
                // 找到了存放记录的非空闲位置
                return;
            }
        }
    }
}

/**
 * @brief ​ 判断是否到达文件末尾
 */
bool RmScan::is_end() const {
    // Todo: 修改返回值
    return rid_.page_no == RM_NO_PAGE;
}

/**
 * @brief RmScan内部存放的rid
 */
Rid RmScan::rid() const {
    return this->rid_;
}