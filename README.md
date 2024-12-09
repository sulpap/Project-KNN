# Project-KNN (2o Παραδοτέο)

Θεοδοσία Παπαδήμα 1115202000162  sdi2000162@di.uoa.gr

Γιώργος Κορύλλος 1115202100069  sdi2100069@di.uoa.gr

Βασιλική Τσαντήλα 1115201800199  sdi1800199@di.uoa.gr

### Οδηγίες εκτέλεσης
Για την συγκεκριμένη εφαρμογή έχουν υλοποιηθεί οι εξής main:
Όσον αφορά τον filteredVamana:
* filtered_main: Φορτώνει τη βάση και κάνει όλους τους υπολογισμούς (FilteredVamana, queries).
* filtered_main_graph: Φορτώνει τη βάση και κάνει το κομμάτι του FilteredVamana, αποθηκεύοντας τον γράφο σε .bin αρχείο.
* config_filtered_main: Ίδια με την filtered_main αλλά τρέχει με configuration file για τις παραμέτρους.

Αντίστοιχα, για τον stitchedVamana:
* stitched_main
* stitched_main_graph
config_stitched_main
Που κάνουν τις ίδιες λειτουργίες.

Όπως επίσης και μια γενική
* main_load: Παίρνει τον γράφο από το .bin αρχείο και κάνει τους υπολογισμούς για τα queries.

Για να τρέξετε την εκάστοτε main:      
Επιλογή 1:
* Μεταφέρεστε στο directory που βρίσκεται το Makefile.
* Εκτελείτε την εντολή: make
* Εκτελείτε την εντολή: chmod +x run_<όνομα της main>.sh
* Εκτελείτε την εντολή: ./run_<όνομα της main>.sh
και αλλάζετε τις παραμέτρους μέσα στο script.

Επιλογή 2:
* Μεταφέρεστε στο directory που βρίσκεται το Makefile.
* Εκτελείτε την εντολή: make
* Εκτελείτε την εντολή: 
	- ./bin/config_filtered_main filtered_config.txt ή
 	- ./bin/config_stitched_main stitched_config.txt
και χειρίζεστε τις τιμές των παραμέτρων μέσω του αντίστοιχου config file.

- To run the `tests`:
    - Πηγαίνουμε στο directory που είναι το Makefile
    - `make test`
    - `./bin/test`
 
- To run the `tests` with `valgrind`:
    - Πηγαίνουμε στο directory που είναι το Makefile
    - `make valgrind`

- To `clean` object and executable files:
    - Πηγαίνουμε στο directory που είναι το Makefile
    - `make clean`

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



+++ άλλες παραδοχές


### Αποτελέσματα & Χρόνοι (Παραδείγματα)

Παράδειγμα εκτέλεσης για k = 20, L = 40, R = 80, a = 1.2:

* Filtered (με τ = 2):
- PC 1: 156.653 seconds or 2.61089 minutes (FilteredVamana: 46.6505 seconds, queries: 108.431 seconds).
- PC 2:
- PC 3:

* Stitched (με R_stitched = 2):
- PC 1: 205.893 seconds or 3.43156 minutes (StitchedVamana: 92.9362 seconds , queries: 111.992 seconds)
- PC 2:
- PC 3:

Δεν υπάρχουν διαρροές μνήμης.
