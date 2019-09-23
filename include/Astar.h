#ifndef PLANNER_ASTAR_H
#define PLANNER_ASTAR_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <vector>

#include "carsize.h"
#include "collision_checking.h"
#include "expanddef.h"
#include "state.h"
#include "typedef.h"

/* Astar class */
class Astar {
 public:
  Astar(int row, int columns, Point2i_type start, Point2i_type goal,
        const std::set<ID_SIZE>& obstacle_list);
  Astar(const Astar&) = delete;

  typedef std::priority_queue<CmpInfo, std::vector<CmpInfo>,
                              std::greater<CmpInfo>>
      priority_que;
  /* 计算路径 */
  bool ComputePath();

  /* 判断起点车身yaw */
  Yaw_type JudgeStartYaw() const;

  /* 计算h值 */
  inline double GetHeuristicValue(const Point2i_type& pt) const {
    // 切比雪夫距离
    // return std::max(fabs(pt.x - goal_pos_.x), fabs(pt.y - goal_pos_.y));
    return TwoPosDistence(pt, goal_pos_);
  }

  /* 获得neighbor */
  std::vector<CellPosState> GetNeighbor(const CellPosState& pos);

  /* 路径回溯 */
  void RecurPath();

  /* 执行Astar */
  bool Execute();

  /* print result */
  void PrintResult();

  /**** 获得类内成员函数 ****/
  inline int get_row() const { return row_; }
  inline int get_col() const { return col_; }
  inline const Point2i_type& get_start() const { return start_pos_; }
  inline const Point2i_type& get_goal() const { return goal_pos_; }
  inline const std::list<CellPosState>& get_final_path() const {
    return final_path_;
  }
  inline std::vector<ID_SIZE> get_point_path() const { return path_; }
  inline const std::set<ID_SIZE>& get_obstacle() const {
    return obstacle_list_;
  }

 private:
  int row_, col_;
  Point2i_type start_pos_, goal_pos_;
  CellInfo *start_, *goal_;
  std::set<ID_SIZE> obstacle_list_;
  std::list<CellPosState> final_path_;
  std::vector<ID_SIZE> path_;
  std::map<CellPosState, CellInfo> all_cell_info_;

  int expand_cnt_;

  /* 修改cellinfo中isclose信息 */
  inline void SetStateValue(CellInfo* const cell, bool state) {
    cell->inclose = state;
  }

  /* 计算KeyValue */
  inline KeyVaule CalculateKey(const CellInfo* const ptr) const {
    double f = (ptr->value.g + ptr->value.h) == DBL_MAX
                   ? DBL_MAX
                   : (ptr->value.g + ptr->value.h);
    return KeyVaule(f, ptr->value.g);
  }

  /* 计算KeyValue overload*/
  inline KeyVaule CalculateKey(double g, double h) const {
    double f = (g + h) == DBL_MAX ? DBL_MAX : (g + h);
    return KeyVaule(f, g);
  }

  /* 判断是否在界内 */
  inline bool IsInMapBoundry(const Point2i_type& pos) const {
    return pos.x >= 0 && pos.x < row_ && pos.y >= 0 && pos.y < col_;
  }

  /* 判断是否为障碍物 */
  inline bool IsObstacle(const Point2i_type& pos) const {
    return obstacle_list_.find(CodeID(pos, col_)) != obstacle_list_.end();
  }

  /* 判断是否为障碍物 overload +1*/
  inline bool IsObstacle(ID_SIZE id) const {
    return obstacle_list_.find(id) != obstacle_list_.end();
  }

  /* 针对26.5度方向的扩展点 */
  template <class T = Point2i_type>
  void NeiborsPush_26_5(std::vector<CellPosState>& neighbors, const T& pos,
                        T (*first)(const T&), T (*second)(const T&));
  /* 针对垂直方向 */
  bool NeiborsPush(std::vector<CellPosState>& neighbors,
                   const Point2i_type& pos,
                   Point2i_type (*first)(const Point2i_type&), Yaw_type yaw);

  /* 两点间距离 */
  inline double TwoPosDistence(const Point2i_type& pt1,
                               const Point2i_type& pt2) const {
    return sqrt(pow((pt1.x - pt2.x), 2.0) + pow((pt1.y - pt2.y), 2.0));
  }

  /* 两点间距离 overload */
  inline double TwoPosDistence(ID_SIZE id1, ID_SIZE id2) {
    return TwoPosDistence(DecodeID(id1, col_), DecodeID(id2, col_));
  }

  /* push openlist */
  bool PushOpenlist(const CellInfo* const curr_cell,
                    const CellPosState& neighbor, priority_que& openlist);

  /* Is in path list */
  inline bool IsInPath(ID_SIZE id) {
    return std::find(path_.begin(), path_.end(), id) != path_.end();
  }

  inline void PrintPosState(const CellPosState& view) {
    PrintCellposState(view, col_);
  }
};

#endif