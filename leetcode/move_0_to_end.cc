/*
283. 移动零
简单
相关标签
相关企业
提示
给定一个数组 nums，编写一个函数将所有 0 移动到数组的末尾，同时保持非零元素的相对顺序。

请注意 ，必须在不复制数组的情况下原地对数组进行操作。

 

示例 1:

输入: nums = [0,1,0,3,12]
输出: [1,3,12,0,0]
示例 2:

输入: nums = [0]
输出: [0]

提示:
1 <= nums.length <= 104
-231 <= nums[i] <= 231 - 1

*/

#include <bits/stdc++.h>
#include <cstddef>
#include <utility>
#include <vector>

using namespace std;

class Solution {
    public:
        void static swap(vector<int> & nums, size_t a, size_t b)  {

            nums[a] = nums[a] ^ nums[b];
            nums[b] = nums[a] ^ nums[b];
            nums[a] = nums[a] ^ nums[b];
        }

        void moveZeroes(vector<int>& nums) {
            
            if(nums.empty() || nums.size() == 1) return;
            
            for(size_t i = 0; i < nums.size(); ++i) {
                if ( nums[i] != 0) continue;
                for(size_t j = i+1 ; j < nums.size(); ++j) {
                    if(nums[j] != 0) {
                        //交换
                        swap(nums,i,j);
                        break;
                    }
                }
            }
            
        }
    };