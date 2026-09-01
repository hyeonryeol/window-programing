// Copyright (c) 2026 Yoon Seong Kim. All rights reserved.

#pragma once

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace kysai {

struct Config {
  static constexpr int kBoardSize = 19;
  static constexpr int kCellCount = kBoardSize * kBoardSize;
  static constexpr int kCandidateRadius = 2;

  static constexpr int kAttackRefinementTimeMs = 350;
  static constexpr int kDefenceTimeMs = 3000;
  static constexpr int kMaximumDefenceThreads = 4;
  static constexpr int kMaximumDepth = 18;
  static constexpr int kQuiescenceDepth = 4;

  static constexpr int kRootQuietMoveLimit = 28;
  static constexpr int kInnerQuietMoveLimit = 16;
  static constexpr int kQuiescenceMoveLimit = 10;
  static constexpr int kTacticalCandidateScanLimit = 48;

  static constexpr int kInfinity = 32'000'000;
  static constexpr int kMateScore = 30'000'000;
  static constexpr int kMaximumStaticScore = 5'000'000;
  static constexpr std::size_t kTranspositionTableSize = 1U << 19;

  static constexpr int kOpenFourBonus = 160'000;
  static constexpr int kOpenThreeBonus = 4'000;
  static constexpr int kAspirationWindow = 1'500;
  static constexpr int kTranspositionMoveBonus = 8'000'000;
  static constexpr int kWinningMoveBonus = 16'000'000;
  static constexpr int kBlockingMoveBonus = 12'000'000;
  static constexpr int kForcingMoveThreshold = 35'000;

  inline static constexpr std::array<int, 6> kLineWeights = {
      0, 3, 24, 240, 18'000, 800'000};
  inline static constexpr std::array<int, 6> kMoveWeights = {
      0, 6, 48, 700, 45'000, 1'000'000};
};

enum class Stone : std::uint8_t {
  kEmpty = 0,
  kBlack = 1,
  kWhite = 2,
};

enum class Bound : std::uint8_t {
  kNone = 0,
  kExact = 1,
  kLower = 2,
  kUpper = 3,
};

constexpr Stone Opponent(Stone stone) {
  return stone == Stone::kBlack ? Stone::kWhite : Stone::kBlack;
}

constexpr int ToPosition(int x, int y) {
  return y * Config::kBoardSize + x;
}

constexpr int PositionX(int position) {
  return position % Config::kBoardSize;
}

constexpr int PositionY(int position) {
  return position / Config::kBoardSize;
}

constexpr bool IsInside(int x, int y) {
  return x >= 0 && x < Config::kBoardSize && y >= 0 &&
         y < Config::kBoardSize;
}

struct Direction {
  int dx;
  int dy;
};

inline constexpr std::array<Direction, 4> kDirections = {
    Direction{1, 0}, Direction{0, 1}, Direction{1, 1}, Direction{1, -1}};

class Geometry {
 public:
  static const Geometry& Get() {
    static const Geometry geometry;
    return geometry;
  }

  const std::vector<std::vector<int>>& Lines() const { return lines_; }

  const std::array<int, 4>& CellLines(int position) const {
    return cell_lines_[position];
  }

 private:
  Geometry() {
    for (auto& line_ids : cell_lines_) {
      line_ids.fill(-1);
    }

    for (int y = 0; y < Config::kBoardSize; ++y) {
      AddLine(0, 0, y, 1, 0);
    }
    for (int x = 0; x < Config::kBoardSize; ++x) {
      AddLine(1, x, 0, 0, 1);
    }
    for (int start_x = 0; start_x < Config::kBoardSize; ++start_x) {
      AddLine(2, start_x, 0, 1, 1);
    }
    for (int start_y = 1; start_y < Config::kBoardSize; ++start_y) {
      AddLine(2, 0, start_y, 1, 1);
    }
    for (int start_x = 0; start_x < Config::kBoardSize; ++start_x) {
      AddLine(3, start_x, Config::kBoardSize - 1, 1, -1);
    }
    for (int start_y = Config::kBoardSize - 2; start_y >= 0; --start_y) {
      AddLine(3, 0, start_y, 1, -1);
    }
  }

  void AddLine(int direction, int x, int y, int dx, int dy) {
    const int line_id = static_cast<int>(lines_.size());
    std::vector<int> line;
    while (IsInside(x, y)) {
      const int position = ToPosition(x, y);
      line.push_back(position);
      cell_lines_[position][direction] = line_id;
      x += dx;
      y += dy;
    }
    lines_.push_back(std::move(line));
  }

  std::vector<std::vector<int>> lines_;
  std::array<std::array<int, 4>, Config::kCellCount> cell_lines_{};
};

class Zobrist {
 public:
  static const Zobrist& Get() {
    static const Zobrist zobrist;
    return zobrist;
  }

  std::uint64_t Piece(int position, Stone stone) const {
    return pieces_[position][static_cast<int>(stone) - 1];
  }

  std::uint64_t Side(Stone stone) const {
    return side_[static_cast<int>(stone) - 1];
  }

 private:
  static std::uint64_t SplitMix64(std::uint64_t& state) {
    std::uint64_t value = (state += 0x9e3779b97f4a7c15ULL);
    value = (value ^ (value >> 30U)) * 0xbf58476d1ce4e5b9ULL;
    value = (value ^ (value >> 27U)) * 0x94d049bb133111ebULL;
    return value ^ (value >> 31U);
  }

  Zobrist() {
    std::uint64_t state = 0x4f4d4f4b42415454ULL;
    for (auto& cell : pieces_) {
      cell[0] = SplitMix64(state);
      cell[1] = SplitMix64(state);
    }
    side_[0] = SplitMix64(state);
    side_[1] = SplitMix64(state);
  }

  std::array<std::array<std::uint64_t, 2>, Config::kCellCount> pieces_{};
  std::array<std::uint64_t, 2> side_{};
};

class Board {
 public:
  Board() { Reset(); }

  void Reset() {
    cells_.fill(Stone::kEmpty);
    neighbour_counts_.fill(0);
    history_.clear();
    hash_ = 0;
    black_score_ = 0;
    white_score_ = 0;

    const std::size_t line_count = Geometry::Get().Lines().size();
    black_line_scores_.assign(line_count, 0);
    white_line_scores_.assign(line_count, 0);
  }

  Stone At(int position) const { return cells_[position]; }

  Stone At(int x, int y) const {
    return IsInside(x, y) ? cells_[ToPosition(x, y)] : Stone::kEmpty;
  }

  bool IsEmpty(int position) const {
    return position >= 0 && position < Config::kCellCount &&
           cells_[position] == Stone::kEmpty;
  }

  int MoveCount() const { return static_cast<int>(history_.size()); }
  std::uint64_t Hash() const { return hash_; }

  void Place(int position, Stone stone) {
    UpdateAffectedLines(position, false);
    cells_[position] = stone;
    hash_ ^= Zobrist::Get().Piece(position, stone);
    history_.push_back(position);
    UpdateNeighbours(position, 1);
    UpdateAffectedLines(position, true);
  }

  void Undo() {
    const int position = history_.back();
    const Stone stone = cells_[position];
    UpdateAffectedLines(position, false);
    UpdateNeighbours(position, -1);
    history_.pop_back();
    hash_ ^= Zobrist::Get().Piece(position, stone);
    cells_[position] = Stone::kEmpty;
    UpdateAffectedLines(position, true);
  }

  bool IsCandidate(int position) const {
    return IsEmpty(position) && neighbour_counts_[position] > 0;
  }

  bool WouldWin(int position, Stone stone) const {
    if (!IsEmpty(position)) {
      return false;
    }
    for (const Direction direction : kDirections) {
      const int length = HypotheticalRunLength(position, stone, direction);
      if ((stone == Stone::kBlack && length == 5) ||
          (stone == Stone::kWhite && length >= 5)) {
        return true;
      }
    }
    return false;
  }

  bool HasWinningLine(int position, Stone stone) const {
    for (const Direction direction : kDirections) {
      const int length = PlacedRunLength(position, stone, direction);
      if ((stone == Stone::kBlack && length == 5) ||
          (stone == Stone::kWhite && length >= 5)) {
        return true;
      }
    }
    return false;
  }

  bool IsForbiddenBlack(int position) {
    if (!IsEmpty(position)) {
      return true;
    }

    int active_directions = 0;
    bool can_create_overline = false;
    for (const Direction direction : kDirections) {
      int nearby_black = 0;
      for (int step = -4; step <= 4; ++step) {
        if (step == 0) {
          continue;
        }
        const int x = PositionX(position) + direction.dx * step;
        const int y = PositionY(position) + direction.dy * step;
        nearby_black += IsInside(x, y) && At(x, y) == Stone::kBlack ? 1 : 0;
      }
      active_directions += nearby_black >= 2 ? 1 : 0;
      can_create_overline =
          can_create_overline ||
          HypotheticalRunLength(position, Stone::kBlack, direction) >= 6;
    }
    if (!can_create_overline && active_directions < 2) {
      return false;
    }

    Place(position, Stone::kBlack);
    const bool exact_five = HasWinningLine(position, Stone::kBlack);
    const bool forbidden = !exact_five &&
                           (HasOverline(position) ||
                            CountFourDirections(position, Stone::kBlack) >= 2 ||
                            CountOpenThreeDirections(position) >= 2);
    Undo();
    return forbidden;
  }

  int Evaluate(Stone side_to_move) const {
    long long score = static_cast<long long>(black_score_) - white_score_;
    score = std::clamp(score,
                       -static_cast<long long>(Config::kMaximumStaticScore),
                       static_cast<long long>(Config::kMaximumStaticScore));
    return side_to_move == Stone::kBlack ? static_cast<int>(score)
                                         : -static_cast<int>(score);
  }

  int MoveHeuristic(int position, Stone stone) const {
    if (!IsEmpty(position)) {
      return -Config::kInfinity;
    }

    const Stone opponent = Opponent(stone);
    int score = 0;
    for (const Direction direction : kDirections) {
      for (int start = -4; start <= 0; ++start) {
        int own_count = 1;
        int opponent_count = 0;
        bool contains_position = false;
        bool valid = true;

        for (int offset = 0; offset < 5; ++offset) {
          const int step = start + offset;
          const int x = PositionX(position) + direction.dx * step;
          const int y = PositionY(position) + direction.dy * step;
          if (!IsInside(x, y)) {
            valid = false;
            break;
          }
          if (step == 0) {
            contains_position = true;
            continue;
          }
          const Stone cell = At(x, y);
          own_count += cell == stone ? 1 : 0;
          opponent_count += cell == opponent ? 1 : 0;
        }

        if (!valid || !contains_position) {
          continue;
        }
        if (opponent_count == 0) {
          score += Config::kMoveWeights[own_count];
        }
        if (own_count == 1) {
          score += Config::kMoveWeights[opponent_count] * 3 / 4;
        }
      }
    }

    const int centre = Config::kBoardSize / 2;
    const int distance = std::max(std::abs(PositionX(position) - centre),
                                  std::abs(PositionY(position) - centre));
    score += Config::kBoardSize - distance;
    return score;
  }

  int CountFourDirections(int origin, Stone stone) const {
    int count = 0;
    for (const Direction direction : kDirections) {
      if (CountWinningPoints(origin, -1, stone, direction) > 0) {
        ++count;
      }
    }
    return count;
  }

  bool CreatesForcingThreat(int position, Stone stone) {
    Place(position, stone);
    bool forcing = false;
    for (const Direction direction : kDirections) {
      if (CountWinningPoints(position, -1, stone, direction) > 0) {
        forcing = true;
        break;
      }
    }
    Undo();
    return forcing;
  }

  bool HasImmediateWinningMove(Stone stone) const {
    for (int position = 0; position < Config::kCellCount; ++position) {
      if (IsCandidate(position) && WouldWin(position, stone)) {
        return true;
      }
    }
    return false;
  }

 private:
  Stone AtWithHypothetical(int x, int y, int position, Stone stone) const {
    if (!IsInside(x, y)) {
      return Stone::kEmpty;
    }
    const int current = ToPosition(x, y);
    return current == position ? stone : cells_[current];
  }

  int HypotheticalRunLength(int position, Stone stone,
                            Direction direction) const {
    int length = 1;
    for (int sign : {-1, 1}) {
      int x = PositionX(position) + direction.dx * sign;
      int y = PositionY(position) + direction.dy * sign;
      while (IsInside(x, y) && At(x, y) == stone) {
        ++length;
        x += direction.dx * sign;
        y += direction.dy * sign;
      }
    }
    return length;
  }

  int PlacedRunLength(int position, Stone stone, Direction direction) const {
    int length = 1;
    for (int sign : {-1, 1}) {
      int x = PositionX(position) + direction.dx * sign;
      int y = PositionY(position) + direction.dy * sign;
      while (IsInside(x, y) && At(x, y) == stone) {
        ++length;
        x += direction.dx * sign;
        y += direction.dy * sign;
      }
    }
    return length;
  }

  bool HasOverline(int position) const {
    for (const Direction direction : kDirections) {
      if (PlacedRunLength(position, Stone::kBlack, direction) >= 6) {
        return true;
      }
    }
    return false;
  }

  bool WinningLineContains(int winning_position, int required_first,
                           int required_second, Stone stone,
                           Direction direction) const {
    int start_x = PositionX(winning_position);
    int start_y = PositionY(winning_position);
    while (IsInside(start_x - direction.dx, start_y - direction.dy) &&
           AtWithHypothetical(start_x - direction.dx, start_y - direction.dy,
                              winning_position, stone) == stone) {
      start_x -= direction.dx;
      start_y -= direction.dy;
    }

    int length = 0;
    int x = start_x;
    int y = start_y;
    while (IsInside(x, y) &&
           AtWithHypothetical(x, y, winning_position, stone) == stone) {
      ++length;
      x += direction.dx;
      y += direction.dy;
    }

    if ((stone == Stone::kBlack && length != 5) ||
        (stone == Stone::kWhite && length < 5)) {
      return false;
    }

    const auto contains = [&](int required) {
      if (required < 0) {
        return true;
      }
      int check_x = start_x;
      int check_y = start_y;
      for (int i = 0; i < length; ++i) {
        if (ToPosition(check_x, check_y) == required) {
          return true;
        }
        check_x += direction.dx;
        check_y += direction.dy;
      }
      return false;
    };
    return contains(required_first) && contains(required_second);
  }

  int CountWinningPoints(int required_first, int required_second, Stone stone,
                         Direction direction) const {
    int count = 0;
    const int origin_x = PositionX(required_first);
    const int origin_y = PositionY(required_first);
    for (int step = -5; step <= 5; ++step) {
      const int x = origin_x + direction.dx * step;
      const int y = origin_y + direction.dy * step;
      if (!IsInside(x, y)) {
        continue;
      }
      const int position = ToPosition(x, y);
      if (!IsEmpty(position)) {
        continue;
      }
      if (WinningLineContains(position, required_first, required_second, stone,
                              direction)) {
        ++count;
      }
    }
    return count;
  }

  int CountOpenThreeDirections(int origin) {
    int count = 0;
    for (const Direction direction : kDirections) {
      bool found = false;
      const int origin_x = PositionX(origin);
      const int origin_y = PositionY(origin);
      for (int step = -4; step <= 4 && !found; ++step) {
        if (step == 0) {
          continue;
        }
        const int x = origin_x + direction.dx * step;
        const int y = origin_y + direction.dy * step;
        if (!IsInside(x, y)) {
          continue;
        }
        const int extension = ToPosition(x, y);
        if (!IsEmpty(extension)) {
          continue;
        }

        Place(extension, Stone::kBlack);
        const bool legal_extension =
            !HasWinningLine(extension, Stone::kBlack) &&
            !HasOverline(extension) &&
            CountFourDirections(extension, Stone::kBlack) < 2;
        const bool creates_open_four =
            legal_extension &&
            CountWinningPoints(origin, extension, Stone::kBlack, direction) >= 2;
        Undo();
        found = creates_open_four;
      }
      count += found ? 1 : 0;
    }
    return count;
  }

  int EvaluateLine(const std::vector<int>& line, Stone stone) const {
    if (line.size() < 5) {
      return 0;
    }

    const Stone opponent = Opponent(stone);
    int score = 0;
    for (std::size_t start = 0; start + 5 <= line.size(); ++start) {
      int own_count = 0;
      bool blocked = false;
      for (std::size_t offset = 0; offset < 5; ++offset) {
        const Stone cell = cells_[line[start + offset]];
        own_count += cell == stone ? 1 : 0;
        blocked = blocked || cell == opponent;
      }
      if (!blocked) {
        score += Config::kLineWeights[own_count];
      }
    }

    for (std::size_t start = 0; start + 6 <= line.size(); ++start) {
      if (cells_[line[start]] != Stone::kEmpty ||
          cells_[line[start + 5]] != Stone::kEmpty) {
        continue;
      }
      int own_count = 0;
      bool blocked = false;
      for (std::size_t offset = 1; offset < 5; ++offset) {
        const Stone cell = cells_[line[start + offset]];
        own_count += cell == stone ? 1 : 0;
        blocked = blocked || cell == opponent;
      }
      if (!blocked && own_count == 4) {
        score += Config::kOpenFourBonus;
      } else if (!blocked && own_count == 3) {
        score += Config::kOpenThreeBonus;
      }
    }
    return score;
  }

  void UpdateAffectedLines(int position, bool add_new_scores) {
    const Geometry& geometry = Geometry::Get();
    for (const int line_id : geometry.CellLines(position)) {
      if (line_id < 0) {
        continue;
      }

      if (!add_new_scores) {
        black_score_ -= black_line_scores_[line_id];
        white_score_ -= white_line_scores_[line_id];
      }

      const std::vector<int>& line = geometry.Lines()[line_id];
      black_line_scores_[line_id] = EvaluateLine(line, Stone::kBlack);
      white_line_scores_[line_id] = EvaluateLine(line, Stone::kWhite);

      if (add_new_scores) {
        black_score_ += black_line_scores_[line_id];
        white_score_ += white_line_scores_[line_id];
      }
    }
  }

  void UpdateNeighbours(int position, int delta) {
    const int position_x = PositionX(position);
    const int position_y = PositionY(position);
    for (int dy = -Config::kCandidateRadius;
         dy <= Config::kCandidateRadius; ++dy) {
      for (int dx = -Config::kCandidateRadius;
           dx <= Config::kCandidateRadius; ++dx) {
        if (dx == 0 && dy == 0) {
          continue;
        }
        const int x = position_x + dx;
        const int y = position_y + dy;
        if (IsInside(x, y)) {
          const int neighbour = ToPosition(x, y);
          neighbour_counts_[neighbour] = static_cast<std::int16_t>(
              neighbour_counts_[neighbour] + delta);
        }
      }
    }
  }

  std::array<Stone, Config::kCellCount> cells_{};
  std::array<std::int16_t, Config::kCellCount> neighbour_counts_{};
  std::vector<int> history_;
  std::uint64_t hash_ = 0;

  std::vector<int> black_line_scores_;
  std::vector<int> white_line_scores_;
  int black_score_ = 0;
  int white_score_ = 0;
};

struct ScoredMove {
  int position = -1;
  int score = 0;
  bool forcing = false;
};

struct MoveList {
  std::array<ScoredMove, Config::kCellCount> moves{};
  int count = 0;

  void Add(ScoredMove move) { moves[count++] = move; }

  void Sort() {
    std::sort(moves.begin(), moves.begin() + count,
              [](const ScoredMove& lhs, const ScoredMove& rhs) {
                return lhs.score > rhs.score;
              });
  }

};

struct TranspositionEntry {
  std::uint64_t key = 0;
  int value = 0;
  std::int16_t depth = -1;
  std::uint16_t best_position = std::numeric_limits<std::uint16_t>::max();
  Bound bound = Bound::kNone;
  std::uint8_t generation = 0;
};

class TranspositionTable {
 public:
  TranspositionTable() : entries_(Config::kTranspositionTableSize) {}

  void NewSearch() { ++generation_; }

  const TranspositionEntry* Probe(std::uint64_t key) const {
    const TranspositionEntry& entry = entries_[Index(key)];
    return entry.key == key ? &entry : nullptr;
  }

  void Store(std::uint64_t key, int depth, int value, Bound bound,
             int best_position) {
    TranspositionEntry& entry = entries_[Index(key)];
    if (entry.key != key && entry.generation == generation_ &&
        entry.depth > depth + 2) {
      return;
    }
    if (entry.key == key && entry.depth > depth && bound != Bound::kExact) {
      return;
    }

    entry.key = key;
    entry.value = value;
    entry.depth = static_cast<std::int16_t>(depth);
    entry.best_position = best_position >= 0
                              ? static_cast<std::uint16_t>(best_position)
                              : std::numeric_limits<std::uint16_t>::max();
    entry.bound = bound;
    entry.generation = generation_;
  }

 private:
  static std::size_t Index(std::uint64_t key) {
    return static_cast<std::size_t>(key) &
           (Config::kTranspositionTableSize - 1);
  }

  std::vector<TranspositionEntry> entries_;
  std::uint8_t generation_ = 0;
};

struct SearchResult {
  int position = -1;
  int value = -Config::kInfinity;
  int completed_depth = 0;
  std::uint64_t nodes = 0;
  std::array<int, Config::kMaximumDepth + 1> positions_by_depth{};
  std::array<int, Config::kMaximumDepth + 1> values_by_depth{};

  SearchResult() {
    positions_by_depth.fill(-1);
    values_by_depth.fill(-Config::kInfinity);
  }
};

class Searcher {
 public:
  SearchResult FindBestMove(Board& board, Stone side,
                            std::chrono::milliseconds time_budget,
                            int start_depth = 1, int seed_position = -1,
                            int seed_value = 0, int root_partition = 0,
                            int root_partition_count = 1) {
    deadline_ = std::chrono::steady_clock::now() + time_budget;
    stopped_ = false;
    nodes_ = 0;
    root_partition_ = std::max(0, root_partition);
    root_partition_count_ = std::max(1, root_partition_count);
    table_.NewSearch();

    MoveList root_moves = GenerateMoves(board, side, -1, 0, false);
    root_moves = PartitionRootMoves(root_moves);
    SearchResult result;
    if (root_moves.count == 0) {
      return result;
    }
    result.position = board.IsEmpty(seed_position) ? seed_position
                                                   : root_moves.moves[0].position;
    result.value = seed_value;
    result.completed_depth = std::max(0, start_depth - 1);
    if (result.completed_depth <= Config::kMaximumDepth) {
      result.positions_by_depth[result.completed_depth] = result.position;
      result.values_by_depth[result.completed_depth] = result.value;
    }

    if (start_depth <= 1 && board.WouldWin(result.position, side)) {
      result.value = Config::kMateScore;
      result.completed_depth = 1;
      result.positions_by_depth[1] = result.position;
      result.values_by_depth[1] = result.value;
      return result;
    }

    int previous_value = seed_value;
    for (int depth = std::max(1, start_depth);
         depth <= Config::kMaximumDepth; ++depth) {
      int alpha = -Config::kInfinity;
      int beta = Config::kInfinity;
      if (depth >= 4) {
        alpha = std::max(-Config::kInfinity,
                         previous_value - Config::kAspirationWindow);
        beta = std::min(Config::kInfinity,
                        previous_value + Config::kAspirationWindow);
      }

      int best_position = result.position;
      int value = SearchRoot(board, side, depth, alpha, beta, best_position);
      if (stopped_) {
        break;
      }

      if (value <= alpha || value >= beta) {
        value = SearchRoot(board, side, depth, -Config::kInfinity,
                           Config::kInfinity, best_position);
        if (stopped_) {
          break;
        }
      }

      result.position = best_position;
      result.value = value;
      result.completed_depth = depth;
      result.positions_by_depth[depth] = result.position;
      result.values_by_depth[depth] = result.value;
      previous_value = value;
      if (std::abs(value) >= Config::kMateScore - 512) {
        break;
      }
    }
    result.nodes = nodes_;
    return result;
  }

 private:
  static int ToTableValue(int value, int ply) {
    if (value >= Config::kMateScore - 512) {
      return value + ply;
    }
    if (value <= -Config::kMateScore + 512) {
      return value - ply;
    }
    return value;
  }

  static int FromTableValue(int value, int ply) {
    if (value >= Config::kMateScore - 512) {
      return value - ply;
    }
    if (value <= -Config::kMateScore + 512) {
      return value + ply;
    }
    return value;
  }

  bool TimeExpired() {
    if ((nodes_ & 31U) != 0U) {
      return false;
    }
    if (std::chrono::steady_clock::now() < deadline_) {
      return false;
    }
    stopped_ = true;
    return true;
  }

  std::uint64_t PositionKey(const Board& board, Stone side) const {
    return board.Hash() ^ Zobrist::Get().Side(side);
  }

  MoveList PartitionRootMoves(const MoveList& source) const {
    if (root_partition_count_ <= 1) {
      return source;
    }
    MoveList partition;
    for (int index = 0; index < source.count; ++index) {
      if (index % root_partition_count_ == root_partition_) {
        partition.Add(source.moves[index]);
      }
    }
    return partition;
  }

  MoveList GenerateMoves(Board& board, Stone side, int transposition_move,
                         int ply, bool tactical_only) {
    MoveList ordered_moves;
    if (board.MoveCount() == 0) {
      ordered_moves.Add({ToPosition(Config::kBoardSize / 2,
                                    Config::kBoardSize / 2),
                         Config::kInfinity, true});
      return ordered_moves;
    }

    std::array<bool, Config::kCellCount> own_wins{};
    std::array<bool, Config::kCellCount> opponent_wins{};
    int own_win_count = 0;
    int opponent_win_count = 0;
    const Stone opponent = Opponent(side);

    for (int position = 0; position < Config::kCellCount; ++position) {
      if (!board.IsCandidate(position)) {
        continue;
      }
      own_wins[position] = board.WouldWin(position, side);
      opponent_wins[position] = board.WouldWin(position, opponent);
      own_win_count += own_wins[position] ? 1 : 0;
      opponent_win_count += opponent_wins[position] ? 1 : 0;
    }

    for (int position = 0; position < Config::kCellCount; ++position) {
      if (!board.IsCandidate(position)) {
        continue;
      }
      if (own_win_count > 0 && !own_wins[position]) {
        continue;
      }
      if (own_win_count == 0 && opponent_win_count > 0 &&
          !opponent_wins[position]) {
        continue;
      }
      int score = board.MoveHeuristic(position, side);
      if (position == transposition_move) {
        score += Config::kTranspositionMoveBonus;
      }
      if (own_wins[position]) {
        score += Config::kWinningMoveBonus;
      } else if (opponent_wins[position]) {
        score += Config::kBlockingMoveBonus;
      }
      ordered_moves.Add(
          {position, score, own_wins[position] || opponent_wins[position]});
    }

    ordered_moves.Sort();
    if (own_win_count > 0 || opponent_win_count > 0) {
      MoveList legal_moves;
      for (int index = 0; index < ordered_moves.count; ++index) {
        const ScoredMove move = ordered_moves.moves[index];
        if (side == Stone::kBlack && !own_wins[move.position] &&
            board.IsForbiddenBlack(move.position)) {
          continue;
        }
        legal_moves.Add(move);
      }
      return legal_moves;
    }

    const int quiet_limit = tactical_only
                                ? Config::kQuiescenceMoveLimit
                                : (ply == 0 ? Config::kRootQuietMoveLimit
                                            : Config::kInnerQuietMoveLimit);
    MoveList legal_moves;
    int quiet_count = 0;
    for (int index = 0; index < ordered_moves.count; ++index) {
      ScoredMove move = ordered_moves.moves[index];
      if (tactical_only && index >= Config::kTacticalCandidateScanLimit) {
        break;
      }
      if (!tactical_only && quiet_count >= quiet_limit &&
          move.score < Config::kForcingMoveThreshold) {
        break;
      }
      if (side == Stone::kBlack && board.IsForbiddenBlack(move.position)) {
        continue;
      }

      if (tactical_only || move.score >= Config::kForcingMoveThreshold) {
        move.forcing = board.CreatesForcingThreat(move.position, side);
      }
      if (tactical_only && !move.forcing) {
        continue;
      }
      if (!move.forcing && quiet_count >= quiet_limit) {
        continue;
      }
      quiet_count += move.forcing ? 0 : 1;
      legal_moves.Add(move);
      if (tactical_only && legal_moves.count >= quiet_limit) {
        break;
      }
    }
    return legal_moves;
  }

  int SearchRoot(Board& board, Stone side, int depth, int alpha, int beta,
                 int& best_position) {
    const std::uint64_t key = PositionKey(board, side);
    int transposition_move = -1;
    if (const TranspositionEntry* entry = table_.Probe(key)) {
      if (entry->best_position != std::numeric_limits<std::uint16_t>::max()) {
        transposition_move = entry->best_position;
      }
    }

    MoveList moves = GenerateMoves(
        board, side, root_partition_count_ > 1 ? -1 : transposition_move, 0,
        false);
    moves = PartitionRootMoves(moves);
    if (moves.count == 0) {
      return -Config::kMateScore;
    }

    const int original_alpha = alpha;
    int best_value = -Config::kInfinity;
    int local_best = moves.moves[0].position;
    bool first_move = true;

    for (int index = 0; index < moves.count; ++index) {
      if (TimeExpired()) {
        return 0;
      }
      const int position = moves.moves[index].position;
      board.Place(position, side);
      int value;
      if (board.HasWinningLine(position, side)) {
        value = Config::kMateScore - 1;
      } else if (first_move) {
        value = -Negamax(board, Opponent(side), depth - 1, -beta, -alpha, 1,
                         position, side);
      } else {
        value = -Negamax(board, Opponent(side), depth - 1, -alpha - 1, -alpha,
                         1, position, side);
        if (!stopped_ && value > alpha && value < beta) {
          value = -Negamax(board, Opponent(side), depth - 1, -beta, -alpha, 1,
                           position, side);
        }
      }
      board.Undo();
      if (stopped_) {
        return 0;
      }

      first_move = false;
      if (value > best_value) {
        best_value = value;
        local_best = position;
      }
      alpha = std::max(alpha, value);
      if (alpha >= beta) {
        break;
      }
    }

    best_position = local_best;
    const Bound bound = best_value <= original_alpha
                            ? Bound::kUpper
                            : (best_value >= beta ? Bound::kLower : Bound::kExact);
    table_.Store(key, depth, ToTableValue(best_value, 0), bound, local_best);
    return best_value;
  }

  int Negamax(Board& board, Stone side, int depth, int alpha, int beta, int ply,
              int last_position, Stone last_stone) {
    ++nodes_;
    if (TimeExpired()) {
      return 0;
    }
    if (last_position >= 0 && board.HasWinningLine(last_position, last_stone)) {
      return -Config::kMateScore + ply;
    }
    if (depth <= 0) {
      return Quiescence(board, side, alpha, beta, ply,
                        Config::kQuiescenceDepth);
    }

    const std::uint64_t key = PositionKey(board, side);
    const int original_alpha = alpha;
    int transposition_move = -1;
    if (const TranspositionEntry* entry = table_.Probe(key)) {
      if (entry->best_position != std::numeric_limits<std::uint16_t>::max()) {
        transposition_move = entry->best_position;
      }
      if (entry->depth >= depth) {
        const int table_value = FromTableValue(entry->value, ply);
        if (entry->bound == Bound::kExact) {
          return table_value;
        }
        if (entry->bound == Bound::kLower) {
          alpha = std::max(alpha, table_value);
        } else if (entry->bound == Bound::kUpper) {
          beta = std::min(beta, table_value);
        }
        if (alpha >= beta) {
          return table_value;
        }
      }
    }

    MoveList moves =
        GenerateMoves(board, side, transposition_move, ply, false);
    if (moves.count == 0) {
      return -Config::kMateScore + ply;
    }

    int best_value = -Config::kInfinity;
    int best_position = moves.moves[0].position;
    bool first_move = true;
    for (int index = 0; index < moves.count; ++index) {
      const int position = moves.moves[index].position;
      board.Place(position, side);
      int value;
      if (first_move) {
        value = -Negamax(board, Opponent(side), depth - 1, -beta, -alpha,
                         ply + 1, position, side);
      } else {
        int reduction = 0;
        if (depth >= 4 && index >= 6 && !moves.moves[index].forcing) {
          reduction = 1;
        }
        value = -Negamax(board, Opponent(side), depth - 1 - reduction,
                         -alpha - 1, -alpha, ply + 1, position, side);
        if (!stopped_ && value > alpha && reduction > 0) {
          value = -Negamax(board, Opponent(side), depth - 1, -alpha - 1,
                           -alpha, ply + 1, position, side);
        }
        if (!stopped_ && value > alpha && value < beta) {
          value = -Negamax(board, Opponent(side), depth - 1, -beta, -alpha,
                           ply + 1, position, side);
        }
      }
      board.Undo();
      if (stopped_) {
        return 0;
      }

      first_move = false;
      if (value > best_value) {
        best_value = value;
        best_position = position;
      }
      alpha = std::max(alpha, value);
      if (alpha >= beta) {
        break;
      }
    }

    const Bound bound = best_value <= original_alpha
                            ? Bound::kUpper
                            : (best_value >= beta ? Bound::kLower : Bound::kExact);
    table_.Store(key, depth, ToTableValue(best_value, ply), bound,
                 best_position);
    return best_value;
  }

  int Quiescence(Board& board, Stone side, int alpha, int beta, int ply,
                 int remaining_depth) {
    ++nodes_;
    if (TimeExpired()) {
      return 0;
    }

    const int stand_pat = board.Evaluate(side);
    const bool must_defend = board.HasImmediateWinningMove(Opponent(side));
    if (remaining_depth <= 0) {
      return must_defend ? -Config::kMateScore / 2 : stand_pat;
    }
    if (!must_defend) {
      if (stand_pat >= beta) {
        return stand_pat;
      }
      alpha = std::max(alpha, stand_pat);
    }

    MoveList moves = GenerateMoves(board, side, -1, ply, true);
    if (moves.count == 0 && must_defend) {
      return -Config::kMateScore + ply;
    }
    for (int index = 0; index < moves.count; ++index) {
      const int position = moves.moves[index].position;
      board.Place(position, side);
      int value;
      if (board.HasWinningLine(position, side)) {
        value = Config::kMateScore - ply;
      } else {
        value = -Quiescence(board, Opponent(side), -beta, -alpha, ply + 1,
                            remaining_depth - 1);
      }
      board.Undo();
      if (stopped_) {
        return 0;
      }
      if (value >= beta) {
        return value;
      }
      alpha = std::max(alpha, value);
    }
    return alpha;
  }

  TranspositionTable table_;
  std::chrono::steady_clock::time_point deadline_{};
  std::uint64_t nodes_ = 0;
  bool stopped_ = false;
  int root_partition_ = 0;
  int root_partition_count_ = 1;
};

class Engine {
 public:
  void Attack(Stone self, int* x, int* y) {
    EnsureInitialized(self);

    int position = -1;
    const bool has_cached_reply =
        cached_reply_.valid && cached_reply_.board_hash == board_.Hash() &&
        IsLegalMove(cached_reply_.position, self);
    if (self == Stone::kBlack && board_.MoveCount() == 0) {
      position = ToPosition(Config::kBoardSize / 2, Config::kBoardSize / 2);
    } else {
      Searcher& refinement_searcher =
          has_cached_reply ? SearcherAt(cached_reply_.searcher_index)
                           : searcher_;
      const SearchResult result = refinement_searcher.FindBestMove(
          board_, self,
          std::chrono::milliseconds(Config::kAttackRefinementTimeMs),
          has_cached_reply ? cached_reply_.completed_depth + 1 : 1,
          has_cached_reply ? cached_reply_.position : -1,
          has_cached_reply ? cached_reply_.value : 0);
      position = IsLegalMove(result.position, self)
                     ? result.position
                     : (has_cached_reply ? cached_reply_.position : -1);
    }

    if (!IsLegalMove(position, self)) {
      position = FindFallbackMove(self);
    }
    if (position < 0) {
      position = 0;
    }

    *x = PositionX(position);
    *y = PositionY(position);
    if (board_.IsEmpty(position)) {
      board_.Place(position, self);
    }
    cached_reply_.valid = false;
  }

  void Defence(Stone self, int x, int y) {
    EnsureInitialized(self);
    cached_reply_.valid = false;

    if (!IsInside(x, y)) {
      return;
    }
    const int position = ToPosition(x, y);
    if (!board_.IsEmpty(position)) {
      return;
    }

    const Stone opponent = Opponent(self);
    if (opponent == Stone::kBlack &&
        !board_.WouldWin(position, Stone::kBlack) &&
        board_.IsForbiddenBlack(position)) {
      return;
    }
    board_.Place(position, opponent);
    if (board_.HasWinningLine(position, opponent)) {
      return;
    }

    int best_searcher_index = 0;
    const SearchResult result = ParallelDefenceSearch(self, best_searcher_index);
    if (IsLegalMove(result.position, self)) {
      cached_reply_.position = result.position;
      cached_reply_.board_hash = board_.Hash();
      cached_reply_.value = result.value;
      cached_reply_.completed_depth = result.completed_depth;
      cached_reply_.searcher_index = best_searcher_index;
      cached_reply_.valid = true;
    }
  }

 private:
  struct CachedReply {
    int position = -1;
    std::uint64_t board_hash = 0;
    int value = 0;
    int completed_depth = 0;
    int searcher_index = 0;
    bool valid = false;
  };

  int DefenceThreadCount() const {
    const unsigned int hardware_threads = std::thread::hardware_concurrency();
    const int available_threads =
        hardware_threads == 0 ? 1 : static_cast<int>(hardware_threads);
    return std::clamp(available_threads, 1,
                      Config::kMaximumDefenceThreads);
  }

  void EnsureSearchers(int count) {
    while (static_cast<int>(additional_searchers_.size()) < count - 1) {
      additional_searchers_.push_back(std::make_unique<Searcher>());
    }
  }

  Searcher& SearcherAt(int index) {
    return index == 0 ? searcher_ : *additional_searchers_[index - 1];
  }

  SearchResult ParallelDefenceSearch(Stone self, int& best_searcher_index) {
    const int thread_count = DefenceThreadCount();
    EnsureSearchers(thread_count);
    if (thread_count == 1) {
      best_searcher_index = 0;
      return searcher_.FindBestMove(
          board_, self, std::chrono::milliseconds(Config::kDefenceTimeMs));
    }

    std::vector<Board> boards(thread_count, board_);
    std::vector<SearchResult> results(thread_count);
    std::vector<std::thread> workers;
    workers.reserve(thread_count - 1);

    for (int index = 1; index < thread_count; ++index) {
      workers.emplace_back([&, index] {
        results[index] = SearcherAt(index).FindBestMove(
            boards[index], self,
            std::chrono::milliseconds(Config::kDefenceTimeMs), 1, -1, 0,
            index, thread_count);
      });
    }
    results[0] = searcher_.FindBestMove(
        boards[0], self, std::chrono::milliseconds(Config::kDefenceTimeMs), 1,
        -1, 0, 0, thread_count);
    for (std::thread& worker : workers) {
      worker.join();
    }

    int common_depth = Config::kMaximumDepth;
    bool has_result = false;
    std::uint64_t total_nodes = 0;
    for (const SearchResult& result : results) {
      if (result.position < 0) {
        continue;
      }
      has_result = true;
      common_depth = std::min(common_depth, result.completed_depth);
      total_nodes += result.nodes;
    }
    if (!has_result) {
      return SearchResult{};
    }

    SearchResult merged;
    merged.completed_depth = common_depth;
    merged.nodes = total_nodes;
    best_searcher_index = 0;
    for (int index = 0; index < thread_count; ++index) {
      const SearchResult& result = results[index];
      if (result.position < 0) {
        continue;
      }
      const int position = result.positions_by_depth[common_depth];
      const int value = result.values_by_depth[common_depth];
      if (position >= 0 && value > merged.value) {
        merged.position = position;
        merged.value = value;
        best_searcher_index = index;
      }
    }
    return merged;
  }

  void EnsureInitialized(Stone self) {
    if (initialized_ && self_ == self) {
      return;
    }
    board_.Reset();
    self_ = self;
    initialized_ = true;
    cached_reply_.valid = false;
  }

  bool IsLegalMove(int position, Stone stone) {
    if (!board_.IsEmpty(position)) {
      return false;
    }
    return stone != Stone::kBlack || board_.WouldWin(position, stone) ||
           !board_.IsForbiddenBlack(position);
  }

  int FindFallbackMove(Stone stone) {
    int best_position = -1;
    int best_score = -Config::kInfinity;
    for (int position = 0; position < Config::kCellCount; ++position) {
      if (!board_.IsEmpty(position)) {
        continue;
      }
      if (stone == Stone::kBlack && !board_.WouldWin(position, stone) &&
          board_.IsForbiddenBlack(position)) {
        continue;
      }
      const int score = board_.MoveHeuristic(position, stone);
      if (score > best_score) {
        best_score = score;
        best_position = position;
      }
    }
    return best_position;
  }

  Board board_;
  Searcher searcher_;
  std::vector<std::unique_ptr<Searcher>> additional_searchers_;
  CachedReply cached_reply_;
  Stone self_ = Stone::kEmpty;
  bool initialized_ = false;
};

inline Engine g_engine;

inline void WhiteAttack_2023184010(int* x, int* y) {
  g_engine.Attack(Stone::kWhite, x, y);
}

inline void WhiteDefence_2023184010(int x, int y) {
  g_engine.Defence(Stone::kWhite, x, y);
}

inline void BlackAttack_2023184010(int* x, int* y) {
  g_engine.Attack(Stone::kBlack, x, y);
}

inline void BlackDefence_2023184010(int x, int y) {
  g_engine.Defence(Stone::kBlack, x, y);
}

}  // namespace kysai

using kysai::BlackAttack_2023184010;
using kysai::BlackDefence_2023184010;
using kysai::WhiteAttack_2023184010;
using kysai::WhiteDefence_2023184010;
