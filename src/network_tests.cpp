#include "network_tests.h"
#include "getRealTime.h"

#include <unistd.h> // usleep
#include <iomanip>

void my_assert(bool b, std::string str) {
    if (!b) {
        std::cout << str << std::endl;
    }
}

void call_sleep() {
    // gives time to server so they can setup
    // otherwise, the client tries to connect too quickly, and connection is refusedkj:w
    usleep(500000);
}

void test_shutdown(bool is_client) {
    std::cout << "Running shutdown test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        uint8_t key[8] = "goodbye";
        uint8_t val[8] = "friends";
        c.cache_set(cache, key, val, 8);
        c.destroy_cache(cache);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_get(bool is_client) {
    std::cout << "Running get test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        uint8_t key[6] = "hello";
        uint8_t val[6] = "world";
        c.cache_set(cache, key, val, 6);
        uint8_t *res = (uint8_t *) c.cache_get(cache, key);
        assert(!(strcmp((const char *)val, (const char *)res)) && "get test failed");
        c.destroy_cache(cache);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_get_large_val(bool is_client) {
    std::cout << "Running get large val test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        uint8_t key[86] = "this is a pretty long key but its not nearly as long as the value associated with it!";
        uint8_t val[1198] = "Asymmetrical gluten-free pop-up keytar blue bottle pinterest. Banjo chia 3 wolf moon, single-origin coffee kickstarter crucifix dreamcatcher deep v. Meditation tofu bicycle rights 90's distillery. Semiotics art party forage brooklyn meh tofu 8-bit, pitchfork squid selvage messenger bag. Neutra meditation green juice master cleanse mumblecore semiotics, salvia typewriter shabby chic. Drinking vinegar hammock bicycle rights, gluten-free chillwave 3 wolf moon you probably haven't heard of them. Selvage mixtape polaroid pinterest. Locavore meditation tousled, literally hashtag mlkshk truffaut cardigan brunch pabst neutra hoodie. Knausgaard four dollar toast 90's, locavore squid photo booth helvetica thundercats fingerstache readymade seitan truffaut polaroid. Offal everyday carry wayfarers, disrupt lumbersexual literally ramps crucifix austin intelligentsia cred cardigan green juice. Heirloom 3 wolf moon church-key, gastropub lumbersexual iPhone hoodie green juice. Vice mlkshk yr portland tote bag wolf. Photo booth vice leggings, small batch seitan cred portland swag semiotics lo-fi. Readymade gastropub helvetica, 3 wolf moon cornhole tumblr meh freegan deep v 8-bit authentic plaid.";
        c.cache_set(cache, key, val, 1198);

        uint8_t *res = (uint8_t *) c.cache_get(cache, key);
        assert(!(strcmp((const char *)val, (const char *)res)) && "get test failed");
        c.destroy_cache(cache);
    }

    else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_put(bool is_client) {
    std::cout << "Running put test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        uint8_t key[6] = "hello";
        uint8_t val[6] = "world";
        c.cache_set(cache, key, val, 6);
        c.destroy_cache(cache);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_update(bool is_client) {
    std::cout << "Running update test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        uint8_t key[6] = "hello";
        uint8_t val[6] = "world";
        c.cache_set(cache, key, val, strlen((char*) val));

        uint8_t val2[10] = "new_world";
        c.cache_set(cache, key, val2, strlen((char*) val2));

        uint8_t *res = (uint8_t *) c.cache_get(cache, key);
        my_assert(0 == strcmp((const char *)val2, (const char *)res), "get test failed");

        c.destroy_cache(cache);

    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_delete(bool is_client) {
    std::cout << "Running delete test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        uint8_t key[25] = "this key will be deleted";
        uint8_t val[11] = "delete me!";
        c.cache_set(cache, key, val, 11);
        c.cache_delete(cache, key);
        c.destroy_cache(cache);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_head(bool is_client) {
    std::cout << "Running head test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        uint32_t actual_mem_used = 0;

        uint8_t key_1[6] = "hello";
        uint8_t val_1[6] = "world";
        c.cache_set(cache, key_1, val_1, 6);
        actual_mem_used += strlen((char*) val_1);

        uint8_t key_2[8] = "another";
        uint8_t val_2[16] = "keyvaluepair";
        c.cache_set(cache, key_2, val_2, 16);
        actual_mem_used += strlen((char*) val_2);

        //we inserted two items, no evictions expected
        uint64_t res_mem_used = c.cache_space_used(cache);

        my_assert(actual_mem_used == res_mem_used, "memory used is not correct");
        c.destroy_cache(cache);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void test_memsize(bool is_client) {
    std::cout << "Running memsize test" << std::endl;
    if (is_client) {
        call_sleep();
        Client c;
        cache_t cache = c.create_cache(15000);
        c.destroy_cache(cache);
    } else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}


void time_get(bool is_client) {
    std::cout << "Running time_get test" << std::endl;
    if (is_client) {
        call_sleep();
        int num_requests = 1000;
        Client c;
        cache_t cache = c.create_cache(15000);
        uint8_t key[6] = "hello";
        uint8_t val[6] = "world";
        c.cache_set(cache, key, val, 6);

        // create one checksum to hold the first character of the response
        // and another which is populated in the following for-loop by the first char of val
        uint8_t chk_sum[num_requests];
        uint8_t chk_sum_correct[num_requests];

        //create the "correct" checksum
        for (int i = 0; i < num_requests; i++){
           chk_sum_correct[i] = val[0];
        }

        // start the timer
        double start_time = getRealTime();
        for (int i = 0; i < num_requests; i++){
            uint8_t *res = (uint8_t *) c.cache_get(cache, key);
            chk_sum[i] = res[0];
        }
        double end_time = getRealTime();

        //get avg request time in ns
        double avg_get_time = (end_time-start_time)/(double)num_requests;

        //make sure the checksums are equivalent
        for (int i = 0; i < num_requests; i++){
            assert(chk_sum[i] == chk_sum_correct[i]);
        }

        std::cout << std::setprecision(18)<< std::setw(18) << "start_time time: "<< start_time << " end_time: " << end_time << std::endl;
        std::cout << "average get request time (ns): "<< avg_get_time << "\n" << std::endl;

        //cleanup
        c.destroy_cache(cache);
    }

    else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void time_get_large_pair(bool is_client) {
    std::cout << "Running time_get test" << std::endl;
    if (is_client) {
        call_sleep();
        int num_requests = 1000;
        Client c;
        cache_t cache = c.create_cache(15000);
        uint8_t key[86] = "this is a pretty long key but its not nearly as long as the value associated with it!";
        uint8_t val[1198] = "Asymmetrical gluten-free pop-up keytar blue bottle pinterest. Banjo chia 3 wolf moon, single-origin coffee kickstarter crucifix dreamcatcher deep v. Meditation tofu bicycle rights 90's distillery. Semiotics art party forage brooklyn meh tofu 8-bit, pitchfork squid selvage messenger bag. Neutra meditation green juice master cleanse mumblecore semiotics, salvia typewriter shabby chic. Drinking vinegar hammock bicycle rights, gluten-free chillwave 3 wolf moon you probably haven't heard of them. Selvage mixtape polaroid pinterest. Locavore meditation tousled, literally hashtag mlkshk truffaut cardigan brunch pabst neutra hoodie. Knausgaard four dollar toast 90's, locavore squid photo booth helvetica thundercats fingerstache readymade seitan truffaut polaroid. Offal everyday carry wayfarers, disrupt lumbersexual literally ramps crucifix austin intelligentsia cred cardigan green juice. Heirloom 3 wolf moon church-key, gastropub lumbersexual iPhone hoodie green juice. Vice mlkshk yr portland tote bag wolf. Photo booth vice leggings, small batch seitan cred portland swag semiotics lo-fi. Readymade gastropub helvetica, 3 wolf moon cornhole tumblr meh freegan deep v 8-bit authentic plaid.";
        c.cache_set(cache, key, val, 1198);

        // create one checksum to hold the first character of the response
        // and another which is populated in the following for-loop by the first char of val
        uint8_t chk_sum[num_requests];
        uint8_t chk_sum_correct[num_requests];

        //create the "correct" checksum
        for (int i = 0; i < num_requests; i++){
           chk_sum_correct[i] = val[0];
        }

        // start the timer
        double start_time = getRealTime();
        for (int i = 0; i < num_requests; i++){
            uint8_t *res = (uint8_t *) c.cache_get(cache, key);
            chk_sum[i] = res[0];
        }
        double end_time = getRealTime();

        //get avg request time in ns
        double avg_get_time = (end_time-start_time)/(double)num_requests;

        //make sure the checksums are equivalent
        for (int i = 0; i < num_requests; i++){
            assert(chk_sum[i] == chk_sum_correct[i]);
        }

        std::cout << std::setprecision(18)<< std::setw(18) << "start_time time: "<< start_time << " end_time: " << end_time << std::endl;
        std::cout << "average get request time (ns): "<< avg_get_time << "\n" << std::endl;

        //cleanup
        c.destroy_cache(cache);
    }

    else {
        Server s;
        s.start();
    }
    std::cout << "... done\n" << std::endl;
}

void run_tests(bool is_client) { 
    test_memsize(is_client); 
    test_put(is_client); 
    test_get(is_client);
    test_get_large_val(is_client);
    test_delete(is_client);
    test_shutdown(is_client);
    test_head(is_client); 
    test_update(is_client); 
}

void run_time_tests(bool is_client) {
    time_get(is_client);
    // time_get_large_pair(is_client);
}

void network_tests(std::string user) {
    std::cout << "****************************" << std::endl;
    std::cout << "~  Running network_tests  ~" << std::endl;
    std::cout << "****************************\n" << std::endl;

    if (user == "client") {
        run_tests(true);
    } else {
        run_tests(false);
    }

    std::cout << "********************************" << std::endl;
    std::cout << "~  Running network_time_tests  ~" << std::endl;
    std::cout << "********************************\n" << std::endl;

    if (user == "client") {
        run_time_tests(true);
    } else {
        run_time_tests(false);
    }

    std::cout << "********************************" << std::endl;
    std::cout << "~  Finished running all tests  ~" << std::endl;
    std::cout << "********************************\n" << std::endl;
}
