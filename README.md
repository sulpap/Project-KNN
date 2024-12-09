# Project-KNN (2o Παραδοτέο)

Θεοδοσία Παπαδήμα 1115202000162  sdi2000162@di.uoa.gr

Γιώργος Κορύλλος 1115202100069  sdi2100069@di.uoa.gr

Βασιλική Τσαντήλα 1115201800199  sdi1800199@di.uoa.gr

### Οδηγίες εκτέλεσης
ΣΗΜΑΝΤΙΚΟ: όλα θα εκτελούνται από το directory που βρίσκεται το Makefile.

Makefile:
- make: Φτιάχνει τα ακόλουθα εκτελέσιμα στο directory ./bin/: -calculate_groundtruth
                                                              -filtered_main_graph
                                                              -stitched_main_graph
                                                              -filtered_main
                                                              -stitched_main
                                                              -first_main (1o Παραδοτέο)

- make test: Συνδυάζει όλα τα test σε ένα εκτελέσιμο στο directory ./bin/: ./bin/test

- make groundtruth: Φτιάχνει το calculate_groundtruth στο directory ./bin/ και στην συνέχεια φτιάχνει το groundtruth για k = 100

- make graphs: Φτιάχνεια τα filtered_main_graph και stitched_main_graph στο directory ./bin/ και στην συνέχεια φτιάχνει τα graphs και τα maps για L = 100, R = 96, a = 1.2, t = 55 (filtered) , R_stitched = 98 (stitched)

- make valgrind: Κάνει ότι και το make test, μόνο που εκτελεί το ./bin/test με valgrind.

- make clean: Διαγράφει τα directories: -./bin/    (Εκτελέσιμα)
                                        -./build/  (Αντικειμενικά)
    Προσοχή: ΔΕΝ διαγράφει τα αρχεία graphs, maps και groundtruth(gt) που δημιουργούνται στο ./datasets/smallscale

Για την συγκεκριμένη εφαρμογή έχουν υλοποιηθεί οι εξής main:
    * calculate_groundtruth
    * filtered_main_graph
    * stitched_main_graph
    * filtered_main
    * stitched_main

calculate_groundtruth:
Είναι η main που υπολογίζει το groundtruth για ένα k. Το αρχείο ονομάζεται gt_k=x.bin (πχ, για k = 100: gt_k=100.bin) και αποθηκεύεται στο ./datasets/smallscale/. Πρέπει να δίνεται το groundtruth με το path στις main.
Μπορείτε να την τρέξετε με τον εξής τρόπο: ./bin/calculate_groundtruth <k>
Π.χ: ./bin/calculate_groundtruth 100

filtered_main_graph:
Είναι η main που χρησιμοποιεί τον FilteredVamana για να παράξει μόνο, τον γράφο, και το map με τα starting points για κάθε φίλτρο. Δεν ελέγχει queries. Τα αρχεία παίρνουν τις εξής ονομασίες: filtered_graph_L=110_a=1.200000_R=96_taph=55 και filtered_map_L=110_a=1.200000_R=96_taph=55. Όταν θα τα δίνεται στις main, πρέπει να τα δίνεται έτσι, όχι με το path. Τα αρχεία αποθηκεύονται στο ./datasets/smallscale/.
Μπορείτε να την τρέξετε με τον εξής τρόπο: ./bin/filtered_main_graph <L> <R> <a> <t> <base_file_path>
Π.χ.: ./bin/filtered_main_graph 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin

stitched_main_graph:
Είναι η main που χρησιμοποιεί τον StitchedVamana για να παράξει μόνο, τον γράφο, και το map με τα starting points για κάθε φίλτρο. Δεν ελέγχει queries. Τα αρχεία παίρνουν τις εξής ονομασίες: stitched_graph_L=110_a=1.200000_R=96_R_stitched=98 και stitched_map_L=110_a=1.200000_R=96_R_stitched=98. Όταν θα τα δίνεται στις main, πρέπει να τα δίνεται έτσι, όχι με το path. Τα αρχεία αποθηκεύονται στο ./datasets/smallscale/.
Μπορείτε να την τρέξετε με τον εξής τρόπο: ./bin/stitched_main_graph <L> <R> <a> <t> <base_file_path>
Π.χ.: ./bin/stitched_main_graph 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin

filtered_main:
Είναι η main που χρησιμοποιεί τον FilteredVamana.
Μπορείτε να την τρέξετε με 3 τρόπους:
    1. Τα inputs από command line: ./bin/filtered_main <k> <L> <R> <a> <t> <base_file_path> <queries_file_path> <groundtruth_file_path>
        Π.χ.: ./bin/filtered_main 100 110 96 1.2 55 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin
    2. Δίνετε τον γράφο έτοιμο: ./bin/filtered_main <k> <graph_filename> <map_filename> <queries_file_path> <groundtruth_file_path>
        Προσοχή: Το graph_filename και το map_filename, πρέπει να είναι τα ονόματα των αρχείων όχι το path, όπως στο groundtruth_file_path!
        Π.χ: ./bin/filtered_main 100 filtered_graph_L=110_a=1.200000_R=96_taph=55 filtered_map_L=110_a=1.200000_R=96_taph=55 ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin
    3. Δίνεται ένα config αρχείο: ./bin/filtered_main <input_file>
        To config αρχείο έχει συγκεκριμένη μορφή!
            Π.χ.:   k = 100
                    L = 110
                    R = 96
                    a = 1.2
                    t = 55
                    base_file = ./datasets/smallscale/dummy-data.bin
                    queries_file = ./datasets/smallscale/dummy-queries.bin
                    groundtruth_file = ./datasets/smallscale/gt_k=100.bin
        Π.χ.: ./bin/filtered_main ./filtered_config.txt
        Μπορείτε να βάλτε πολλά configurations όμως θα πρέπει να βάζετε το σύμβολο '#' μπροστά από τις γραμμές τους και να βγάζετε το'#' μόνο από το configuration που θέλετε να χρησιμοποιήσετε. H σειρά και η θέση των παραμέτρων δεν έχουν σημασία, ούτε τα whitespaces, αρκεί να είναι στην ίδια γραμμή.
        Για παράδειγμα, αυτό είναι δεκτό:
                                        k = 100
                                        #L = 110
                                        L = 130
                                        #L = 150
                                        R = 96
                                        a = 1.2
                                        #t = 55
                                        base_file = ./datasets/smallscale/dummy-data.bin
                                        #t = 55
                                        queries_file = ./datasets/smallscale/dummy-queries.bin
                                        t = 55
                                        groundtruth_file = ./datasets/smallscale/gt_k=100.bin

stitched_main:
Είναι η main που χρησιμοποιεί τον StitchedVamana.
Μπορείτε να την τρέξετε με 3 τρόπους:
    1. Τα inputs από command line: ./bin/stitched_main <k> <L> <R> <a> <R_stitched> <base_file_path> <queries_file_path> <groundtruth_file_path>
        Π.χ.: ./bin/stitched_main 100 110 96 1.2 98 ./datasets/smallscale/dummy-data.bin ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin
    2. Δίνετε τον γράφο έτοιμο: ./bin/stitched_main <k> <graph_filename> <map_filename> <queries_file_path> <groundtruth_file_path>
        Προσοχή: Το graph_filename και το map_filename, πρέπει να είναι τα ονόματα των αρχείων όχι το path, όπως στο groundtruth_file_path!
        Π.χ: ./bin/stitched_main 100 stitched_graph_L=110_a=1.200000_R=96_R_stitched=98 stitched_map_L=110_a=1.200000_R=96_R_stitched=98 ./datasets/smallscale/dummy-queries.bin ./datasets/smallscale/gt_k=100.bin
    3. Δίνεται ένα config αρχείο: ./bin/stitched_main <input_file>
        To config αρχείο έχει συγκεκριμένη μορφή!
            Π.χ.:   k = 100
                    L = 110
                    R = 96
                    a = 1.2
                    R_stitched = 98
                    base_file = ./datasets/smallscale/dummy-data.bin
                    queries_file = ./datasets/smallscale/dummy-queries.bin
                    groundtruth_file = ./datasets/smallscale/gt_k=100.bin
        Π.χ.: ./bin/stitched_main ./stitched_config.txt
        Μπορείτε να βάλτε πολλά configurations όμως θα πρέπει να βάζετε το σύμβολο '#' μπροστά από τις γραμμές τους και να βγάζετε το'#' μόνο από το configuration που θέλετε να χρησιμοποιήσετε. H σειρά και η θέση των παραμέτρων δεν έχουν σημασία, ούτε τα whitespaces, αρκεί να είναι στην ίδια γραμμή.
        Για παράδειγμα, αυτό είναι δεκτό:
                                        k = 100
                                        #L = 110
                                        L = 130
                                        #L = 150
                                        R = 96
                                        a = 1.2
                                        #R_stitched = 98
                                        base_file = ./datasets/smallscale/dummy-data.bin
                                        #R_stitched = 98
                                        queries_file = ./datasets/smallscale/dummy-queries.bin
                                        R_stitched = 98
                                        groundtruth_file = ./datasets/smallscale/gt_k=100.bin

Μπορείτε επίσης να δείτε τα bash_script αρχεία, ώστε να είναι πιο εύκολο να το γράψετε ή μπορείτε να τρέξετε το ίδιο το bash_script αρχείο.

### Διαχωρισμός εργασιών

Ο καθένας ανέλαβε από κάποια βααικά κομμάτια (filteredGreedySearch, filteredRobustPrune, filtered & stitchedVamana), καθώς και από έναν αριθμό επιμέρους βοηθητικών συναρτήσεων, μαζί με τα unit test που τους αντιστοιχούν.

### Σχεδιαστικές επιλογές

* Αναφορικά με τις filteredVamana & findMedoid: Λόγω της παραλλαγής του ενός label ανά κόμβο, η υλοποίηση ήταν απλούστερη από τον ψευδοκώδικα. Έτσι, για τις εν λόγω συναρτήσεις υπάρχει λειτουργική υλοποίηση σύμφωνα αυστηρά με τον ψευδοκώδικα σε σχόλιο, και απλοποιημένη υλοποίηση (εκτός σχολίου), η οποία είναι ταχύτερη.

* Αναφορικά με τον stitchedVamana: η κλήση της RobustPrune είναι αναγκαία στην περίπτωση που η ένωση των υπογράφων δημιουργήσει νέες ακμές στον συνολικό γράφο και άρα το κλάδεμα ορισμένων ακμών κρίνεται απαραίτητο. Εν προκειμένω, παρατηρήθηκε ότι μετά την ένωση των υπογράφων δεν δημιουργούνται νέες ακμές, γι'αυτό και η κλήση της RobustPrune δεν χρειάζεται. Έτσι, αποφασίζουμε η κλήση αυτή να μην πραγματοποιείται και η vamana του 1ου παραδοτέου να καλείται με παράμετρο Rstitched αντί για Rsmall [(piazza post)](https://piazza.com/class/m1kh0ggogpyg0/post/72). 

* Αναφορικά με τα unfiltered queries: οι vamanas αυτού του παραδοτέου παράγουν έναν υπογράφο για κάθε φίλτρο. Αυτοί οι υπογράφοι δεν συνδέονται μεταξύ τους.    
  Αν θέλουμε να υπολογίσουμε τους k-ANNS ενός unfiltered query, τότε καλούμε την filteredGreedy με S = {starting nodes για κάθε φίλτρο} και F = {κάθε φίλτρο}. Η greedy θα ξεκινήσει την αναζήτηση με το starting node που βρίσκεται πιο κοντά στο query. Η greedy θα αλλάξει υπογράφο μόνο όταν δεν υπάρχει γείτονας του starting node πιο κοντά στο query από τα άλλα starting nodes.    
  Αντιλαμβανόμαστε ότι το παραπάνω δίνει χαμηλά ποσοστά επιτυχίας στα unfiltered queries. Έτσι, ο τρόπος που το διορθώνουμε αυτό είναι: τρέχουμε την filteredGreedy με S = {points που προέκυψαν τρέχοντας τον greedy για k = 1 για κάθε φίλτρο με starting point το αντίστοιχο starting point του φίλτρου} και F = {κάθε φίλτρο} [(piazza post)](https://piazza.com/class/m1kh0ggogpyg0/post/70)   
  Ο παραπάνω τροποποιημένος υπολογισμός γίνεται είτε ο γράφος έχει φτιαχτεί από τον filteredVamana, είτε ο γράφος έχει φτιαχτεί από τον stitchedVamana.   

* Αναφορικά με τις συναρτήσεις που αποθηκεύουν τον γράφο σε binary μορφή αλλά και που τον διαβάζουν από binary μορφή: χρησιμοποιούμε το graph_binary_io.cpp. Στο αρχείο αυτό θα βρούμε επιπλέον και συναρτήσεις ανάγνωσης και αποθήκευσης του Map<int>, <Node*> (map που υπολογίζεται από την findMedoid) από/σε binary μορφή.
    
    Ειδικότερα, επισημαίνουμε τον τρόπο με τον οποίο η save_graph_to_binary αποθηκεύει τον γράφο:
    - number of nodes N
    - For each node:
        - Node ID
        - Node's Graph ID (int)
        - Coordinates 
        - Label
        - Number of neighbors M
        - IDs of neighbors

    *Αποθηκεύουμε πρώτα τον αριθμό των κόμβων και ύστερα, για κάθε κόμβο: το ID του, την ετικέτα, τις συντεταγμένες, τον αριθμό των γειτόνων του και τα ID των γειτόνων.*

    Τέλος, επισημαίνουμε και τον τρόπο με τον οποίο η save_map_to_binary αποθηκεύει τον map των starting nodes:
  - number of nodes in M
  - For each node:
        - Label, Node ID

* Αναφορικά με τις συναρτήσεις ανάγνωσης των datasets του παραδοτέου: η databin_read διαβάζει τα points του dataset και τα επιστρέφει σε vectors δίχως το πεδίο T. Αντίστοιχα, η queries_bin διαβάζει τα points του dataset και τα επιστρέφει σε vectors δίχως τα πεδία l και r. Τέλος, η ground_truth υπολογίζει το ground truth με exhaustive search επιστρέφοντας σε vectors τα ids των points που αποτελούν λύση για το εκάστοτε query.


### Αποτελέσματα & Χρόνοι (Παραδείγματα)

Παράδειγμα εκτέλεσης για k = 20, L = 40, R = 80, a = 1.2:

* Filtered (με τ = 2):
    - PC 1: 156.653 seconds or 2.61089 minutes (FilteredVamana: 46.6505 seconds, queries: 108.431 seconds)
    - PC 2: 40.6549 seconds or 0.677581 minutes (FilteredVamana: 13.2469 seconds, queries: 27.2346 seconds), Total Accuracy 98.8043% (Filtered: 99.9703%, Unfiltered: 97.7126%)
    - PC 3: 121.669 seconds or 2.02782 minutes (FilteredVamana: 39.4146 seconds, queries: 81.5089 seconds), Total Accuracy 98.8372% (Filtered: 99.9576%, Unfiltered: 97.7882%%)

* Stitched (με R_stitched = 2):
    - PC 1: 205.893 seconds or 3.43156 minutes (StitchedVamana: 92.9362 seconds , queries: 111.992 seconds)
    - PC 2: 54.5899 seconds or 0.909831 minutes (StitchedVamana: 26.0254 seconds, queries: 28.3942 seconds) Total Accuracy 98.9059% (Filtered: 99.9915%, Unfiltered: 97.8895%)
    - PC 3: 160.149 seconds or 2.66915 minutes (StitchedVamana: 81.3318 seconds , queries: 78.1706 seconds), Total Accuracy 98.9152% (Filtered: 99.9894%, Unfiltered: 97.9094%)

Παράδειγμα εκτέλεσης για k = 100, L = 110, R = 96, a = 1.2:

* Filtered (με τ = 55):
    - PC 1: 814.093 seconds or 13.5682 minutes (FilteredVamana: 318.533 seconds, queries: 0.0984857 seconds)
    - PC 2: 182.838 seconds or 3.04731 minutes (FilteredVamana: 65.9162 seconds, queries: 116.697 seconds), Total Accuracy 99.5714% (Filtered: 99.9972%, Unfiltered: 99.2023%)
    - PC 3: 561.533 seconds or 9.35888 minutes (FilteredVamana: 202.391 seconds, queries: 358.215 seconds), Total Accuracy 99.571% (Filtered: 99.9972%, Unfiltered: 99.2015%)

* Stitched (με R_stitched = 98):
    - PC 1: 954.395 seconds or 15.9066 minutes (StitchedVamana: 385.931 seconds, queries: 567.07 seconds)
    - PC 2: 190.3 seconds or 3.17166 minutes (StitchedVamana: 75.8819 seconds, queries: 114.179 seconds), Total Accuracy 99.6017% (Filtered: 99.6267%, Unfiltered: 99.2576%)
    - PC 3: 607.317 seconds or 10.1219 minutes (StitchedVamana: 254.685 seconds, queries: 351.685 seconds), Total Accuracy 99.5836% (Filtered: 99.9982%, Unfiltered: 99.2242%)


Δεν υπάρχουν διαρροές μνήμης.