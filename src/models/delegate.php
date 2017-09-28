<?php  
class Delegate extends \Illuminate\Database\Eloquent\Model {  
    protected $primaryKey = 'delegate';
    protected $table = 'delegates';
    protected $fillable = ['*'];
    public $timestamps = false;
}
