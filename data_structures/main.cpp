#include <list>
#include <iostream>
#include <vector>
#include <functional>
#include <iterator>
#include <algorithm>
#include <cmath>

int main() {
  using namespace std;
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  
  int n;
  cin >> n;

  std::vector<int> lst(n);
  std::unordered_map<int, int> mp;

  for (int i = 0, x; i < n; i++) {
    cin >> lst[i];
    mp[i] = i; 
  }


  for(int i = 0, ind; i < n; i++) {
    cin >> ind;
    ind--;
    
    int val;
    // remove the ind th element
    int pos = mp[ind];

    val = lst[pos];

    lst[pos] = lst.back();
    lst.pop_back();
    mp[lst[pos]] = pos;
    mp.erase(ind);

    std::cout << val << ' ';
  }






  return 0;
}