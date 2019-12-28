#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <array>
#include <list>

typedef std::array<std::array<int, 4>, 4> mdarray;

class puzzle
{
	//int m_board[4][4];
	mdarray m_board;
	int m_goal_board[4][4];
	std::map<int, std::pair<int, int>> m_helper;
	std::list<mdarray> m_used_boards;	
	void backtrack(std::vector<char>& a, int k);
	bool is_solution(std::vector<char>& a, int k);
	void process_solution(std::vector<char>& a, int k);
	void construct_candidates(std::vector<char>& a, int k, std::vector<char>& candidates);
	void make_move(std::vector<char>& a, int k, char candidate);
	void unmake_move(std::vector<char>& a, int k, char candidate);
	
	void print_goal_board();
	bool is_solvable();
	bool m_finished;
	int manhattan_distance();
	int manhattan_distance(char move);
	bool is_valid_move(char candidate);
  public:
	puzzle();
	void solve(const std::vector<int>& vals);	
	void print_board();
};

int 
puzzle::manhattan_distance(char move)
{
	
	int m_board_after_move[4][4];
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			m_board_after_move[i][j] = m_board[i][j];
		}
	}

	int mdist = 0;

	int blank_row = -1;
	int blank_col = -1;
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			int val = m_board[i][j];
			if (!val)
			{
				blank_row = i;	
				blank_col = j;
				break;
			}
		}
		if (blank_row != -1)
			break;
	}


	switch(move)
	{
		case 'R':
		{
			int rcol = blank_col + 1;
			m_board_after_move[blank_row][blank_col] = m_board_after_move[blank_row][rcol] ;
			m_board_after_move[blank_row][rcol] = 0;
		}
		break;	

		case 'L':
		{
			int lcol = blank_col - 1;
			m_board_after_move[blank_row][blank_col] = m_board_after_move[blank_row][lcol]; 
			m_board_after_move[blank_row][lcol] = 0;
		}
		break;	

		case 'U':
		{
			int urow = blank_row - 1;
			m_board_after_move[blank_row][blank_col] = m_board_after_move[urow][blank_col]; 
			m_board_after_move[urow][blank_col] = 0;
		
		}
		break;	

		case 'D':
		{

			int drow = blank_row + 1;
			m_board_after_move[blank_row][blank_col] = m_board_after_move[drow][blank_col]; 
			m_board_after_move[drow][blank_col] = 0;
		}
		break;	

		default:
			break;
	}
	
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			if (m_board_after_move[i][j])
			{
				auto coord = m_helper[m_board_after_move[i][j]];
				mdist += abs(i-coord.first) + abs(j - coord.second);
			}
		}
	}
	return mdist;
}

int 
puzzle::manhattan_distance()
{
	int mdist = 0;
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			if (m_board[i][j])
			{
				auto coord = m_helper[m_board[i][j]];
				mdist += abs(i-coord.first) + abs(j - coord.second);
			}
		}
	}
	return mdist;
}

bool
puzzle::is_solvable()
{
	std::set<int> invTracker;
	int invCount = 0;
	int row = 0;
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			int val = m_board[i][j];
			if (!val)
			{
				row = 4-i;
				continue;
			}
			invTracker.insert(val);
			invCount += std::distance(++invTracker.find(val), invTracker.end());
		}
	}
	bool rowEven = false;
	bool invCountEven = false;
	if (!(invCount%2))
		invCountEven = true;
	if (!(row%2))
		rowEven = true;	
	if ((rowEven && invCountEven) || (!rowEven && !invCountEven))
		return false;
	return true;
}

void 
puzzle::construct_candidates(std::vector<char>& a, int k, std::vector<char>& candidates)
{
	int blank_row = -1;
	int blank_col = -1;
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			int val = m_board[i][j];
			if (!val)
			{
				blank_row = i;	
				blank_col = j;
				break;
			}
		}
		if (blank_row != -1)
			break;
	}
	bool rvalid{true}, lvalid{true}, uvalid{true}, dvalid{true};
	if (!blank_col)
	{
		lvalid = false;
	}

	if (blank_col == 3)
	{
		rvalid = false;
	}

	if (!blank_row)
	{
		uvalid = false;
	}

	if (blank_row == 3)
	{
		dvalid = false;
	}
	
	if (k > 0)
	{
		switch(a[k-1])
		{
			case 'L':
			{
				rvalid = false;	
			}
			break;
			
			case 'R':
			{
				lvalid = false;	
			}
			break;

			case 'U':
			{
				dvalid = false;
			}
			break;

			case 'D':
			{
				uvalid = false;
			}
			break;


			default:
				break;
		}
	}

	std::vector<char> moves;
	if (rvalid)
		moves.push_back('R');
	if (lvalid)
		moves.push_back('L');
	if (uvalid)
		moves.push_back('U');
	if (dvalid)
		moves.push_back('D');

	std::multimap<int, char> mdists;
	
	for (auto move : moves)
	{
		mdists.insert(std::pair(manhattan_distance(move) + k, move));
	}

	for (auto mdist : mdists)
	{
		if (is_valid_move(mdist.second))
		{
			candidates.push_back(mdist.second);		
		}
	}




}

bool 
puzzle::is_valid_move(char candidate)
{
	int blank_row = -1;
	int blank_col = -1;
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			int val = m_board[i][j];
			if (!val)
			{
				blank_row = i;	
				blank_col = j;
				break;
			}
		}
		if (blank_row != -1)
			break;
	}

	mdarray board = m_board;

	switch(candidate)
	{
		case 'R':
		{
			int rcol = blank_col + 1;
			board[blank_row][blank_col] = board[blank_row][rcol] ;
			board[blank_row][rcol] = 0;
		}
		break;	

		case 'L':
		{
			int lcol = blank_col - 1;
			board[blank_row][blank_col] = board[blank_row][lcol]; 
			board[blank_row][lcol] = 0;
		}
		break;	

		case 'U':
		{
			int urow = blank_row - 1;
			board[blank_row][blank_col] = board[urow][blank_col]; 
			board[urow][blank_col] = 0;
		
		}
		break;	

		case 'D':
		{

			int drow = blank_row + 1;
			board[blank_row][blank_col] = board[drow][blank_col]; 
			board[drow][blank_col] = 0;
		}
		break;	

		default:
			break;
	}

	for (const auto& used_board : m_used_boards)
	{
		if (used_board == board)
			return false;
	}
	return true;
}

void 
puzzle::make_move(std::vector<char>& a, int k, char candidate)
{
	int blank_row = -1;
	int blank_col = -1;
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			int val = m_board[i][j];
			if (!val)
			{
				blank_row = i;	
				blank_col = j;
				break;
			}
		}
		if (blank_row != -1)
			break;
	}


	switch(candidate)
	{
		case 'R':
		{
			int rcol = blank_col + 1;
			m_board[blank_row][blank_col] = m_board[blank_row][rcol] ;
			m_board[blank_row][rcol] = 0;
		}
		break;	

		case 'L':
		{
			int lcol = blank_col - 1;
			m_board[blank_row][blank_col] = m_board[blank_row][lcol]; 
			m_board[blank_row][lcol] = 0;
		}
		break;	

		case 'U':
		{
			int urow = blank_row - 1;
			m_board[blank_row][blank_col] = m_board[urow][blank_col]; 
			m_board[urow][blank_col] = 0;
		
		}
		break;	

		case 'D':
		{

			int drow = blank_row + 1;
			m_board[blank_row][blank_col] = m_board[drow][blank_col]; 
			m_board[drow][blank_col] = 0;
		}
		break;	

		default:
			break;
	}

	m_used_boards.push_back(m_board);
}

void 
puzzle::unmake_move(std::vector<char>& a, int k, char candidate)
{
	int blank_row = -1;
	int blank_col = -1;
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			int val = m_board[i][j];
			if (!val)
			{
				blank_row = i;	
				blank_col = j;
				break;
			}
		}
		if (blank_row != -1)
			break;
	}
	
	m_used_boards.remove(m_board);

	switch(candidate)
	{
		case 'R':
		{
			int rcol = blank_col - 1;
			m_board[blank_row][blank_col] = m_board[blank_row][rcol] ;
			m_board[blank_row][rcol] = 0;
		}
		break;	

		case 'L':
		{
			int lcol = blank_col + 1;
			m_board[blank_row][blank_col] = m_board[blank_row][lcol]; 
			m_board[blank_row][lcol] = 0;
		}
		break;	

		case 'U':
		{
			int urow = blank_row + 1;
			m_board[blank_row][blank_col] = m_board[urow][blank_col]; 
			m_board[urow][blank_col] = 0;
		
		}
		break;	

		case 'D':
		{

			int drow = blank_row - 1;
			m_board[blank_row][blank_col] = m_board[drow][blank_col]; 
			m_board[drow][blank_col] = 0;
		}
		break;	

		default:
			break;
	}
}

puzzle::puzzle()
{
	int val = 0;
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			m_goal_board[i][j] = ++val;
		}
	}
	m_goal_board[3][3] = 0;
	m_finished = false;
	m_helper[1] = std::pair(0, 0);
	m_helper[2] = std::pair(0, 1);
	m_helper[3] = std::pair(0, 2);
	m_helper[4] = std::pair(0, 3);

	m_helper[5] = std::pair(1, 0);
	m_helper[6] = std::pair(1, 1);
	m_helper[7] = std::pair(1, 2);
	m_helper[8] = std::pair(1, 3);

	m_helper[9] = std::pair(2, 0);
	m_helper[10] = std::pair(2, 1);
	m_helper[11] = std::pair(2, 2);
	m_helper[12] = std::pair(2, 3);

	m_helper[13] = std::pair(3, 0);
	m_helper[14] = std::pair(3, 1);
	m_helper[15] = std::pair(3, 2);
	m_helper[0] = std::pair(3, 3);
}

bool 
puzzle::is_solution(std::vector<char>& a, int k)
{
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			if (m_goal_board[i][j] != m_board[i][j])
			{
				return false;
			}
		}
	}
	return true;
}

void 
puzzle::process_solution(std::vector<char>& a, int k)
{
	std::cout << "found solution " << std::endl;
	std::cout << "moves=";
	for (int i=0; i <= k ; ++i)
		std::cout << a[i] ;
	std::cout << std::endl;
	m_finished = true;
}

void 
puzzle::backtrack(std::vector<char>& a, int k)
{
	if (is_solution(a, k))
	{
		process_solution(a, k);
	}
	else
	{
		k += 1;
		std::vector<char> candidates;
		construct_candidates(a, k, candidates);
		for (auto candidate : candidates)
		{
			a[k] = candidate;
			make_move(a, k, candidate);
			backtrack(a, k);
			if (m_finished)
				return;
			unmake_move(a, k, candidate);
		}
	}
}

void
puzzle::print_goal_board()
{
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			std::cout << m_goal_board[i][j] << " ";
		}
		std::cout << std::endl;
	}
}


void
puzzle::print_board()
{
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			std::cout << m_board[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void 
puzzle::solve(const std::vector<int>& vals)
{
	int l = 0;
	for(int i=0; i < 4; ++i)
	{
		for (int j=0; j < 4; ++j)
		{
			m_board[i][j] = vals[l++];
		}
	}

	if (!is_solvable())
	{
		std::cout << "This puzzle is not solvable." << std::endl;
		return;
	}

	std::vector<char> a(500, 'X');
	int k = -1;
	backtrack(a, k);
}

int main()
{
	puzzle pz;
	std::vector<int> vals {2, 3, 4, 0, 1, 5, 7, 8, 9, 6, 10, 12, 13, 14, 11, 15};
	std::vector<int> vals2 {13, 1, 2, 4, 5, 0, 3, 7, 9, 6, 10, 12, 15,8, 11, 14};
	std::vector<int> vals3 {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,14, 0, 15}; 
	std::vector<int> vals4 {5, 0, 6, 3, 9, 7, 2, 1, 10, 12, 8, 4, 13, 14, 11, 15};
	pz.solve(vals4);
	return 0;
}

