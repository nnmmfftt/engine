// Copyright [2018] Alibaba Cloud All rights reserved
#include "engine_race.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <map>
#include "util.h"
#include <pthread.h>
#include <data_store.h>
namespace polar_race {
static const char kLockFile[] = "LOCK";
RetCode Engine::Open(const std::string& name, Engine** eptr) {
  return EngineRace::Open(name, eptr);
}

Engine::~Engine() {
}

/*
 * Complete the functions below to implement you own engine
 */

// 1. Open engine
RetCode EngineRace::Open(const std::string& name, Engine** eptr) {
  *eptr = NULL;
  EngineRace *engine_race = new EngineRace(name);
  RetCode ret = engine_race->plate_.Init();
  if (ret != kSucc) {
    delete engine_race;
    return ret;
  }
  ret = engine_race->store_.Init();
//  pthread_t tid;
//  tid =	pthread_self();
//  printf("The current thread id is : %u \n " ,(uint32_t)tid);
  if (ret != kSucc) {
    delete engine_race;
    return ret;
  }

  if (0 != LockFile(name + "/" + kLockFile, &(engine_race->db_lock_))) {
    delete engine_race;
    return kIOError;
  }
  *eptr = engine_race;
  return kSucc;
}

// 2. Close engine
EngineRace::~EngineRace() {
  if (db_lock_) UnlockFile(db_lock_);
}

// 3. Write a key-value pair into engine
RetCode EngineRace::Write(const PolarString& key, const PolarString& value) {
//  pthread_mutex_lock(&mu_);
  Location location;
  location.file_no =compute_file_no();
  RetCode ret = store_.Append(value.ToString(), &location);
  if (ret == kSucc) {
    ret = plate_.AddOrUpdate(key.ToString(), location);
  }
//  pthread_mutex_unlock(&mu_);
  return kSucc;
  
}

// 4. Read value of a key
RetCode EngineRace::Read(const PolarString& key, std::string* value) {
//  pthread_mutex_lock(&mu_);
  Location location;
  RetCode ret = plate_.Find(key.ToString(), &location);
  if (ret == kSucc) {
    value->clear();
    ret = store_.Read(location, value);
  }
//  pthread_mutex_unlock(&mu_);
  
  return kSucc;
}

/*
 * NOTICE: Implement 'Range' in quarter-final,
 *         you can skip it in preliminary.
 */
// 5. Applies the given Vistor::Visit function to the result
// of every key-value pair in the key range [first, last),
// in order
// lower=="" is treated as a key before all keys in the database.
// upper=="" is treated as a key after all keys in the database.
// Therefore the following call will traverse the entire database:
//   Range("", "", visitor)
RetCode EngineRace::Range(const PolarString& lower, const PolarString& upper,
    Visitor &visitor) {
  return kSucc;
}

}  // namespace polar_race
