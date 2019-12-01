pysplit: src/pysplit.cpp src/pinyin_language_model.h src/pinyin_tire_tree.h src/pinyin_utils.h
	g++ -std=c++11 src/pysplit.cpp -o pysplit
clean:
	rm pysplit