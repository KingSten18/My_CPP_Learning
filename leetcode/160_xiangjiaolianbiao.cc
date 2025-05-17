/*
*/
#include  <bits/stdc++.h>

using namespace std;



  struct ListNode {
      int val;
      ListNode *next;
      ListNode(int x) : val(x), next(NULL) {}
  };

 class Solution {
    public:
        ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
            
            set<ListNode*> listA;
            ListNode *p = headA;
            while (p) {
                listA.insert(p);
                p = p->next;
            }

            p = headB;
            while(p) {

                 
                
                if (listA.find(p) != listA.end()) {
                    return p;
                }
                p = p->next;
            }
            return nullptr;
        }
    };