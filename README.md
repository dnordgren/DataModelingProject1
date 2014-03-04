#Data Modeling - Project 1
####March 3, 2014 - Spring 2014 Section 001
Sawyer Jager, Rees Klintworth, Derek Nordgren, Brad Steiner

##Contents
* Report
* Queries - located in Code/Queries - contains queries e - h
* Sorts - located in Code/Sorts - contains functions used to sort tables
* Data model - located in Code/Data_models - contains models of User, Message, and Location tables
* Utility functions - located in Code/Utility - contains the functions used to generate tables from provided data

##Parameters used to operate sorting functions
* sort_location.c - Param: 0 for sort by state, 1 for sort by locationID
* sort_message.c - Param: 0 for sort by date:time, 1 for sort by hour:min, 2 for sort by messageID, 3 for sort by userID
* sort_location.c - Param: 0 for sort by ID, 1 for sort by locationID, 2 for sort by message_num
