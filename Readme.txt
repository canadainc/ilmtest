CREATE TABLE questions (id INTEGER PRIMARY KEY, standard_body TEXT NOT NULL, ordered_body TEXT, count_body TEXT, before_body TEXT, after_body TEXT, suite_page_id INTEGER REFERENCES suite_pages(id) ON DELETE CASCADE ON UPDATE CASCADE);
1, "Which of the following are pillars of faith?", "Rearrange the pillars of faith in order", "How many pillars of faith are there?", "Which pillar of faith comes right before ['Belief in Books']", "Which pillar of faith comes right after ['Belief in Books']"
2, "Which of the following are obligatory prayers?", "Rearrange the obligatory prayers in order", "How many obligatory prayers are there each day?", "Which prayer is the next one after ['Fajr']", "Which prayer is the one before ['Isha']"
3, "How many people were at the camp of the Khawaarij when Ibn Abbaas went to them?"
4, ""

CREATE TABLE choices (id INTEGER PRIMARY KEY, value TEXT);
1,Belief in Allah
2,Belief in Last Day
3,Belief in Books
4,Belief in Angels,
5,Belief in Divine Decree
6,Belief in His Messengers
7,Duha
8,Qiyam al-Layl
9,Fajr
10,Dhuhr
11,Asr,
12,Maghrib,
13,Isha
14,"6000"

CREATE TABLE answers (id INTEGER PRIMARY KEY, question_id INTEGER REFERENCES questions(id) ON DELETE CASCADE ON UPDATE CASCADE, choice_id INTEGER REFERENCES choices(id) INTEGER, sort_order INTEGER, wrong INTEGER);
1,1,1,1
2,1,4,2
3,1,6,3
4,1,3,4
5,1,2,5
6,1,5,6
7,2,7,,1
8,2,8,,1
9,2,9,1
10,2,10,2
11,2,11,3
12,2,12,4
13,2,13,5
14,3,14