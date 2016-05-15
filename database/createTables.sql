CREATE TABLE IF NOT EXISTS products(
	id		INTEGER		PRIMARY KEY NOT NULL,
	name		TEXT		NOT NULL,
	quantity	INTEGER		NOT NULL DEFAULT 0 check(quantity >= 0),
	price		FLOAT		NOT NULL,
	description	TEXT
);

CREATE TABLE IF NOT EXISTS orders(
	id		INTEGER PRIMARY KEY NOT NULL,   -- Auto increments by default
	total		FLOAT 	NOT NULL,
	address		TEXT,
  	`time`		INTEGER NOT NULL -- Unix time
);

CREATE TABLE IF NOT EXISTS orderEntry(
	id          INTEGER PRIMARY KEY NOT NULL,       -- Auto increments by default
	order_id    INTEGER NOT NULL,
	product_id  INTEGER NOT NULL,
	quantity    INTEGER NOT NULL DEFAULT 1,
	FOREIGN KEY (order_id) REFERENCES orders(id),
	FOREIGN KEY (product_id) REFERENCES products(id)
);

INSERT INTO products VALUES(1, "Vodka", 60, 30.5, "A Russian favourite imported from the heart of the coldest place on Earth, Serbia");
INSERT INTO products VALUES(2, "Gin", 30, 40, "A Dutch favourite");
INSERT INTO products VALUES(3, "Chianti", 40, 90, "Aged wine imported from the cuore of Tuscany, Italy");
INSERT INTO products VALUES(4, "Fernet", 100, 25, "Want to get drunk at a pregame with your amigos? Nothing beats the terrible hangover that Fernet gives you");
INSERT INTO products VALUES(5, "Quilmes", 200, 20, "You aren't Argentine until you've drunk Quilmes, the most famous beer in la Tierra de la Plata");