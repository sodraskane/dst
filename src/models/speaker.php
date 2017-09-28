<?php  
class Speaker extends \Illuminate\Database\Eloquent\Model {
    protected $primaryKey = 'speaker';
    protected $table = 'speakers';
    protected $fillable = ['*'];
    public $timestamps = false;

    public function delegate() {
        return $this->belongsTo('Delegate', 'delegate');
    }
}
