class Password {
    public int length;
    public boolean write;
    public String password;
    public String filename;

    public Password(int length, boolean write) {
        this.length = length;
        this.write = write;
    }

    public Password(int length, boolean write, String filename) {
        this.length = length;
        this.write = write;
        this.filename = filename;
    }
}
