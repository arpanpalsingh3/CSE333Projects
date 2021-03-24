# Compilation variables
CXX = g++
CXXFLAGS = -Wall -g -std=c++17

# Edit this to name the directory where the data files live
# if you're not running on attu.
DATA_DIR=/cse/courses/cse333/21wi/public/anc-masc-data

# Default target builds the app.
word-frequency: word-frequency.cc
	$(CXX) $(CXXFLAGS) $< -o word-frequency

word-pair-frequency: word-pair-frequency.cc
	$(CXX) $(CXXFLAGS) $< -o word-pair-frequency	

# Run the app on both data sets.
# Part A requires modifying these rules
run: word-frequency
	@echo -e "\nSpoken"
	@./word-frequency <$(DATA_DIR)/all-spoken.txt | sort -r -g -k 2 | head -n 25
	@echo -e "\nWritten"
	@./word-frequency <$(DATA_DIR)/all-written.txt | sort -r -g -k 2 | head -n 25

run-pair: word-pair-frequency
	@echo -e "\nSpoken"
	@./word-pair-frequency <$(DATA_DIR)/all-spoken.txt | sort -r -g -k 3 | head -n 25
	@echo -e "\nWritten"
	@./word-pair-frequency <$(DATA_DIR)/all-written.txt | sort -r -g -k 3 | head -n 25

run-pair-solution: word-pair-frequency-solution
	@echo -e "\nSpoken"
	@./word-pair-frequency-solution <$(DATA_DIR)/all-spoken.txt | sort -r -g -k 3 | head -n 25
	@echo -e "\nWritten"
	@./word-pair-frequency-solution <$(DATA_DIR)/all-written.txt | sort -r -g -k 3 | head -n 25

# clean is a "phony" target (doesn't produce a like named file)
.PHONY: clean
clean:
	rm -f word-frequency *~
