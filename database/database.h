/* 
 * File:   database.h
 * Author: juan_
 *
 * Created on April 13, 2016, 10:07 AM
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <product.h>
#include <order.h>

/**
 * Queries the database and returns all products, including those with no stock.
 * 
 * @param connection A database connection through which to get data.
 * @param destArray Where to store the retrieved products.
 * @return int The number of retrieved products.
 */
int db_get_products(sqlite3* connection, Product **destArray);

/**
 * Attempts to place the specified order. If there is enough stock of every
 * requested product, the order is placed. Otherwise, the order is modified to
 * represent the largest satisfiable order.
 * 
 * @param connection A database connection.
 * @param order The order to place. <b>NOTE: </b>Will be modified if the order
 * could not be satisfied.
 * @return int 1 if the order was placed successfully, 0 if it wasn't. Note that
 * in this case <i>order</i> will have been modified.
 */
int db_place_order(sqlite3* connection, Order order);

#endif /* DATABASE_H */

