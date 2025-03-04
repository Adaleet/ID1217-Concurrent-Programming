import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class HoneyBeesAndBear {

    public static final int H = 10;   
    public static final int NUM_BEES = 5; 

    public static void main(String[] args) {
        HoneyPot pot = new HoneyPot(H);
        
        Bear bear = new Bear(pot); // starting the bear thread.
        bear.start();
        
        for (int i = 1; i <= NUM_BEES; i++) {         // Start the bee threads.
            Bee bee = new Bee(pot, i);
            bee.start();
        }
    }
}

/**
 * The monitor representing the pot of honey, using a ReentrantLock with two condition variables:
 * - notFull: bees wait here if the pot is full.
 * - full: the bear waits here until the pot is full.
 */
class HoneyPot {
    private final int capacity;
    private int count; 
    private final Lock lock;
    private final Condition notFull;
    private final Condition full;

    public HoneyPot(int capacity) { 
        this.capacity = capacity;
        this.count = 0;
        this.lock = new ReentrantLock(true);          // Using new ReentrantLock(true) for strict fairness.
        this.notFull = lock.newCondition();
        this.full = lock.newCondition();
    }

    /**
     * Called by a bee to add one portion of honey.
     * If the pot is already full, the bee waits until the bear has eaten.
     */
    public void addHoney(int beeId) throws InterruptedException {
        lock.lock(); // lock.lock and lock.unlock ensures mutual exclusion, by whichever that enters the function acquires the lock.
        try {
            while (count == capacity) {
                notFull.await();
            }
            count++;
            System.out.println("Bee " + beeId + " added honey. Pot now has " + count + "/" + capacity + " portions.");
            if (count == capacity) {
                System.out.println("Bee " + beeId + " filled the pot! Waking up the bear...");
                full.signal(); 
            }
        } finally { // ensuring that unlocking always happens
            lock.unlock();
        }
    }

    /**
     * Called by the bear to eat the honey.
     * The bear waits until the pot is full.
     */
    public void eatHoney() throws InterruptedException {
        lock.lock();
        try {
            while (count < capacity) {
                full.await();
            }
            System.out.println("Bear wakes up! Eating all the honey...");
            Thread.sleep(2000);
            count = 0;
            System.out.println("Bear goes back to sleep. Pot is now empty.");
            notFull.signalAll();
        } finally {
            lock.unlock();
        }
    }
}

/**
 * Represents a bee thread that repeatedly gathers honey and adds it to the pot.
 */
class Bee extends Thread {
    private final HoneyPot pot;
    private final int beeId;

    public Bee(HoneyPot pot, int beeId) {
        this.pot = pot;
        this.beeId = beeId;
    }

    @Override
    public void run() {
        try {
            while (true) {
                Thread.sleep((int)(Math.random() * 3000) + 1000); // Simulate the time it takes to gather honey
                pot.addHoney(beeId);
            }
        } catch (InterruptedException e) {
            System.err.println("Bee " + beeId + " was interrupted.");
        }
    }
}

/**
 * Represents the bear thread that sleeps until the pot is full, then eats all the honey.
 */
class Bear extends Thread {

    private final HoneyPot pot;

    public Bear(HoneyPot pot) {
        this.pot = pot;
    }

    @Override
    public void run() {
        try {
            while (true) {
                pot.eatHoney();
            }
        } catch (InterruptedException e) {
            System.err.println("Bear was interrupted.");
        }
    }
}
/*
 * Fairness comment:
 * This solution uses a standard ReentrantLock which by default is not guaranteed to be fair.
 * If strict fairness is desired (i.e., threads waiting longest get the lock next), we can create a 
 * new ReentrantLock(true). However, this might reduce performance. This is for example done in the constructor, 
 * "this.lock = new ReentrantLock(); to this.lock = new ReentrantLock(true);". The trade-off is that fair lock 
 * may reduce performance due to increased scheduling overhead.
 * The monitor itself ensures correct synchronization between the bees and the bear,
 * so all threads will eventually be scheduled. We are also ensuring correct mutual exclusion.
 */
