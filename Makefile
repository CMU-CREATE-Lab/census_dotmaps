all: make_prototile_zoom shuffle_records

make_prototile_zoom: make_prototile_zoom.cpp
	g++ -O3 -Wall -o $@ $<

shuffle_records: shuffle_records.cpp
	g++ -O3 -Wall -o $@ $<
